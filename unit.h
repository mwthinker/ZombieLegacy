#ifndef UNIT_H
#define UNIT_H

#include "input.h"
#include "weapon.h"
#include "physicalunit.h"
#include "state.h"
#include "bullet.h"

#include <functional>
#include <queue>

namespace zombie {

class Unit : public PhysicalUnit {
public:
	enum UnitEvent {SHOOT,RELOADING,DIE};

	Unit(double x, double y, double angle, Weapon weapon, bool infected ,int id);
	virtual ~Unit();

	// Overrides from PhysicalUnit.
	// Returns true if object is to be removed from PhyscalEngine.
	bool toRemove() const override;

	Unit(mw::Packet& packet);

	// Overides Object function.
	mw::Packet generatePacket() const;

	// Simulates the physics at time (time) one time step (timeStep) ahead.
	// Based on the input given.
	void updatePhysics(double time, double timeStep, Input input);
	
	void setState(State state);
	State getState() const;

	// Return the view distance.
	double viewDistance();

	// Return the distance of where all things are being visible, 
	// no matter of orientation.
	double smallViewDistance();

	// Return the current view direction, i.e. the unit's orientation.
	double viewAngle() const;

	// Return true if the point (x, y) is inside the unit.	
	bool isInside(double x, double y) const override;

	// Return true if the point (x, y) is inside the units small view distance,
	// where all things are being visible.
	bool isInsideSmalViewDistance(double x, double y) const;

	// Return true if the point is viewable by the unit, else fals.
	bool isPointViewable(double x, double y);
	
	// Return the angle for the current 
	double moveDirection() const;	

	double healthPoints() const override;
	void updateHealthPoint(double deltaLife) override;

	bool isDead() const override;

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

	void addEventHandler(std::function<void(UnitEvent)> handler);

private:
	void sendEventToHandlers(UnitEvent unitEvent) const;

	void turn(double angle);

	// Physical States.
	double angleVelocity_;
	double angle_;

	// Properties
	double viewDistance_;
	double viewAngle_;
	double smallViewDistance_;

	// Health
	double healthPoints_;
	bool isDead_;
		
	Weapon weapon_;
	bool isInfected_;

	double timeLeftToRun_;

	std::queue<Bullet> bullets_;
	std::vector<std::function<void(UnitEvent)>> eventHandlers_;
};

} // namespace zombie

#endif // UNIT_H