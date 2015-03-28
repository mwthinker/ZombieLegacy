#ifndef UNIT_H
#define UNIT_H

#include "movingobject.h"
#include "input.h"
#include "weapon.h"
#include "state.h"

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

		Unit(float mass, float radius, float life, float walkingSpeed,
			float runningSpeed, bool infected, Position grip, const WeaponPtr& weapon);
		virtual ~Unit();

		Unit(const Unit&);
		Unit& operator=(const Unit&);

		void updatePhysics(double time, double timeStep);

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

		inline void setInput(Input input) {
			input_ = input;
		}

		inline Input getInput() const {
			return input_;
		}

		b2Body* getBody() const override;

		inline Position getGrip() const {
			return grip_;
		}

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

		void signal(int eventType);

		inline virtual void eventHandler(int eventType) {
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
		bool died_;

		WeaponPtr weapon_;
		Position grip_;
		bool isInfected_;

		float timeLeftToRun_;

		b2Body* body_;
		Input input_;
		Input lastInput_;
	};

} // Namespace zombie.

#endif // UNIT_H
