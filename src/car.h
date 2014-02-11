#ifndef CAR_H
#define CAR_H

#include "movingobject.h"
#include "input.h"
#include "state.h"
#include "box2ddef.h"

#include <mw/signal.h>

namespace zombie {

	class Unit;
	class Driver;
	class Building;

	// Defines the property of a car. The car has 4 wheels but is simulated as having 
	// one front wheel and one backwheel in order to simlify the math.
	class Car : public MovingObject {
	public:
		enum CarEvent {
			ACTION,
			MOVED,
			REMOVED
		};

		Car(float mass, float life, float width, float length);
		virtual ~Car();

		Car(const Car& car);
		Car& operator=(const Car& car);

		void createBody(b2World* world, State state);

		Driver* getDriver() const;
		void setDriver(Driver* driver);

		void updatePhysics(float time, float timeStep, Input input) override;

		void applySpin(float impulse) {
			body_->ApplyAngularImpulse(impulse);
		}

		State getState() const override;

		float getWidth() const {
			return width_;
		}

		float getLength() const {
			return length_;
		}

		bool isInsideViewArea(Position position) const override;

		Position getPosition() const {
			return body_->GetPosition();
		}

		b2Body* getBody() const override {
			return body_;
		}

		Weapon getWeapon() const override {
			return Weapon();
		}

		float getDirection() const override {
			return body_->GetAngle();
		}

		bool isInfected() const override {
			// When no driver the car is seen as infected and therefore ignored by the zombies.
			return player_ == nullptr;
		}

		float getViewDistance() const override {
			return 10;
		}

		bool isDead() const override {
			return false;
		}

		mw::signals::Connection addEventHandler(mw::Signal<Car*, CarEvent>::Callback callback) {
			return eventSignal_.connect(callback);
		}

		void collisionWith(Object* ob, float impulse) override {
			ob->collisionWith(this, impulse);
		}

		void collisionWith(Unit*, float impulse) override;

		void collisionWith(Car*, float impulse) override;

		void collisionWith(Building* ob, float impulse) override;

		void destroyBody(b2World* world) override;

		bool isDestroyed() const override;

	private:
		void applyFriction(float frictionForwardFrontWheel, float frictionForwardBackWheel,
			float frictionLateralFrontWheel, float frictionLateralBackWheel);

		b2Vec2 getFrontWheelPosition() const {
			return body_->GetWorldPoint(b2Vec2(length_ * wheelDelta_, 0));
		}

		b2Vec2 getBackWheelPosition() const {
			return body_->GetWorldPoint(b2Vec2(-length_ * wheelDelta_, 0));
		}

		b2Vec2 getDirectionVector() const {
			return body_->GetWorldVector(b2Vec2(std::cos(steeringAngle_), std::sin(steeringAngle_)));
		}

		mw::Signal<Car*, CarEvent> eventSignal_;

		b2Body* body_;
		float steeringAngle_;
		
		float length_, width_;
		float mass_;
		float currentTime_;
		float wheelDelta_;
	};

} // Namespace zombie.

#endif // CAR_H
