#ifndef UNIT_H
#define UNIT_H

#include "movingobject.h"
#include "input.h"
#include "weapon.h"
#include "state.h"

#include <mw/signal.h>

namespace zombie {

	class Unit : public MovingObject {
	public:
		enum UnitEvent {
			DIE,
			INJURED,
			WALK,
			STANDSTILL,
			RUN,
			ACTION
		};

		Unit(float mass, float radius, float life, float walkingSpeed, float runningSpeed, bool infected, const WeaponPtr& weapon);
		virtual ~Unit();

		Unit(const Unit&);
		Unit& operator=(const Unit&);

		void updatePhysics(float time, float timeStep);

		State getState() const;

		void setState(const State& state);

		// Return the view distance.
		float getViewDistance() const;

		// Return the distance of where all things are being visible, 
		// no matter of orientation.
		float smallViewDistance() const;

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

		WeaponPtr getWeapon() const {
			return weapon_;
		}

		void setWeapon(const WeaponPtr& weapon) {
			weapon_ = weapon;
		}

		Position getPosition() const override;

		float getRadius() const;
		
		mw::signals::Connection addEventHandler(mw::Signal<Unit*, UnitEvent>::Callback);

		inline void setInput(Input input) {
			input_ = input;
		}

		inline Input getInput() const {
			return input_;
		}

		b2Body* getBody() const override;

	private:
		void createBody(b2World* world) override;

		void destroyBody() override {
			if (body_ != nullptr) {
				b2World* world = body_->GetWorld();
				if (world != nullptr) {
					world->DestroyBody(body_);
				}
				body_ = nullptr;
			}
		}

		// Properties
		float viewDistance_;
		float viewAngle_;
		float smallViewDistance_;
		float walkingSpeed_;
		float runningSpeed_;
		float radius_;
		float mass_;

		// Health
		float healthPoints_;
		bool isDead_;

		WeaponPtr weapon_;
		bool isInfected_;

		float timeLeftToRun_;
		
		mw::Signal<Unit*, UnitEvent> eventSignal_;

		b2Body* body_;
		Input input_;
		Input lastInput_;
	};

} // Namespace zombie.

#endif // UNIT_H
