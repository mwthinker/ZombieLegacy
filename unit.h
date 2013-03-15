#ifndef UNIT_H
#define UNIT_H

#include "object.h"
#include "input.h"
#include "weapon.h"
#include "state.h"
#include "bullet.h"

#include <Box2D/Box2D.h>

#include <functional>
#include <queue>

namespace zombie {

class Unit : public Object {
public:
	enum UnitEvent {SHOOT,RELOADING,DIE,WALK,STANDSTILL,RUN};

	Unit(float x, float y, float angle, Weapon weapon, bool infected);
	virtual ~Unit();

	// Simulates the physics at time (time) one time step (timeStep) ahead.
	// Based on the input given.
	void updatePhysics(float time, float timeStep, Input input);
	
	void setState(State state);
	State getState() const;

	// Return the view distance.
	float viewDistance();

	// Return the distance of where all things are being visible, 
	// no matter of orientation.
	float smallViewDistance();

	// Return the current view direction, i.e. the unit's orientation.
	float viewAngle() const;

	// Return true if the point (x, y) is inside the unit.	
	bool isInside(float x, float y) const;

	// Return true if the point (x, y) is inside the units small view distance,
	// where all things are being visible.
	bool isInsideSmalViewDistance(float x, float y) const;

	// Return true if the point is viewable by the unit, else fals.
	bool isPointViewable(float x, float y);
	
	// Return the angle for the current 
	float moveDirection() const;	

	float healthPoints() const;
	void updateHealthPoint(float deltaLife);

	bool isDead() const;

	void setIsDead() {
		isDead_ = true;
	}

	bool isInfected() const {
		return isInfected_;
	}

	// The bullet is polled and and saved in (bullet) and true is returned.
	// If no bullet has been shot the function returns false;
	bool pollShot(Bullet& bullet);

	Weapon getWeapon() const {
		return weapon_;
	}

	Position getPosition() const {
		return Position(body_->GetPosition().x,body_->GetPosition().y);
	}

	float radius() const {
		b2Fixture* f = body_->GetFixtureList();             
        b2CircleShape* circle = (b2CircleShape*) f->GetShape();
		return circle->m_radius;        
	}

	void addEventHandler(std::function<void(UnitEvent)> handler);

	b2Body* getBody() const {
		return body_;
	}
private:
	void sendEventToHandlers(UnitEvent unitEvent) const;

	void turn(float angle);

	// Physical States.
	float angleVelocity_;
	float angle_;

	// Properties
	float viewDistance_;
	float viewAngle_;
	float smallViewDistance_;

	// Health
	float healthPoints_;
	bool isDead_;
		
	Weapon weapon_;
	bool isInfected_;

	float timeLeftToRun_;

	std::queue<Bullet> bullets_;
	std::vector<std::function<void(UnitEvent)>> eventHandlers_;

	b2Body* body_;
};

} // namespace zombie

#endif // UNIT_H