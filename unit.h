#ifndef UNIT_H
#define UNIT_H

#include "movingobject.h"
#include "input.h"
#include "weapon.h"
#include "state.h"
#include "bullet.h"

#include <Box2D/Box2D.h>

#include <functional>
#include <boost/signal.hpp>

namespace zombie {

	class Unit : public MovingObject {
	public:
		enum UnitEvent {SHOOT,RELOADING,DIE,WALK,STANDSTILL,RUN};

		Unit(float x, float y, float angle, const Weapon& weapon, bool infected);
		virtual ~Unit();

		// Simulates the physics at time (time) one time step (timeStep) ahead.
		// Based on the input given.
		void updatePhysics(float time, float timeStep, Input input) override;

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

		void kill() {
			isDead_ = true;
		}

		bool isInfected() const {
			return isInfected_;
		}

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

		boost::signals::connection addActionHandler(std::function<void(Unit*)> handler);
		boost::signals::connection addEventHandler(std::function<void(UnitEvent)> handler);
		boost::signals::connection addShootHandler(std::function<void(Unit*, const Bullet& bullet)> handler);

		b2Body* getBody() const override {
			return body_;
		}

		void removeBody() override {
			if (body_ != nullptr) {
				getWorld()->DestroyBody(body_);
				body_ = nullptr;
			}
		}
	private:
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

		boost::signal<void(Unit*)> actionSignal_;
		boost::signal<void(Unit*, Bullet)> shootSignal_;
		boost::signal<void(UnitEvent)> eventSignal_;

		b2Body* body_;
	};

} // namespace zombie

#endif // UNIT_H