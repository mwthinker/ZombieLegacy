#ifndef UNIT_H
#define UNIT_H

#include "movingobject.h"
#include "input.h"
#include "weapon.h"
#include "state.h"
#include "bullet.h"
#include "typedefs.h"

#include <mw/signal.h>

#include <Box2D/Box2D.h>

#include <functional>
#include <list>

namespace zombie {

	class Unit : public MovingObject {
	public:
		enum UnitEvent {
			SHOOT, RELOADING, DIE, WALK, STANDSTILL, RUN
		};

		Unit(const State& state, float mass, float radius, float life, float walkingSpeed, float runningSpeed, bool infected, const Weapon& weapon);
		~Unit();
		
		void updatePhysics(float time, float timeStep, Input input) override;
				
		void collision(float impulse) override;

		State getState() const override;

		// Return the view distance.
		float getViewDistance() const;

		// Return the distance of where all things are being visible, 
		// no matter of orientation.
		float smallViewDistance();

		// Return the current view direction, i.e. the unit's orientation.
		float viewAngle() const;

		// Return true if the point (x, y) is inside the unit.	
		bool isInside(Position position) const;

		bool isInsideViewArea(Position position) const;

		// Return true if the point (x, y) is inside the units small view distance,
		// where all things are being visible.
		bool isInsideSmalViewDistance(Position position) const;

		// Return the angle for the current 
		float getDirection() const;

		float healthPoints() const;
		void updateHealthPoint(float deltaLife);

		bool isDead() const override;

		bool isInfected() const override {
			return isInfected_;
		}

		Weapon getWeapon() const override {
			return weapon_;
		}

		void setWeapon(const Weapon& weapon) {
			weapon_ = weapon;
		}

		Position getPosition() const override {
			return body_->GetPosition();
		}

		float getRadius() const {
			b2Fixture* f = body_->GetFixtureList();
			b2CircleShape* circle = (b2CircleShape*) f->GetShape();
			return circle->m_radius;
		}

		mw::signals::Connection addActionHandler(mw::Signal<Unit*>::Callback);
		mw::signals::Connection addEventHandler(mw::Signal<UnitEvent>::Callback);
		mw::signals::Connection addShootHandler(mw::Signal<Unit*, Bullet>::Callback);

		b2Body* getBody() const override {
			return body_;
		}

	private:
		// Properties
		float viewDistance_;
		float viewAngle_;
		float smallViewDistance_;
		float walkingSpeed_;
		float runningSpeed_;

		// Health
		float healthPoints_;
		bool isDead_;

		Weapon weapon_;
		bool isInfected_;

		float timeLeftToRun_;

		mw::Signal<Unit*> actionSignal_;
		mw::Signal<Unit*, Bullet> shootSignal_;
		mw::Signal<UnitEvent> eventSignal_;

		b2Body* body_;
		Input lastInput_;
	};

} // Namespace zombie.

#endif // UNIT_H
