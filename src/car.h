#ifndef CAR_H
#define CAR_H

#include "movingobject.h"
#include "input.h"
#include "state.h"
#include "unit.h"

#include <mw/signal.h>

#include <Box2D/Box2D.h>

#include <cmath>

namespace zombie {

	enum CarEvent {
		CAREVENT_EXPLODE,
		CAREVENT_ACCELERATE,
		CAREVENT_BRAKE
	};

	class Car : public MovingObject {
	public:
		Car(float x, float y, float angle = 0.0) {
			length_ = 2.0f;
			width_ = 1.0f;
			float mass = 100.0f;

			currentTime_ = 0.0f;
			steeringAngle_ = 0.0f;
			wheelDelta_ = 0.4f;

			idDriver_ = 0;
			
			// Box2d properties.
			b2BodyDef bodyDef;
			bodyDef.type = b2_dynamicBody;
			bodyDef.position.Set(x, y);
			bodyDef.angle = angle;
			body_ = getWorld()->CreateBody(&bodyDef);
			body_->SetUserData(this);
			
			// Body properties.
			{
				b2PolygonShape dynamicBox;
				dynamicBox.SetAsBox(length_, width_);

				b2FixtureDef fixtureDef;
				fixtureDef.shape = &dynamicBox;
				fixtureDef.density = 1.0f;
				fixtureDef.friction = 0.3f;
				b2Fixture* fixture = body_->CreateFixture(&fixtureDef);
				fixture->SetUserData(this);
			}
		}

		~Car() {
			getWorld()->DestroyBody(body_);
		}
		
		int getDriverId() const{
			return idDriver_;
		}

		void setDriver(Unit* unit) {
			if (unit == nullptr) {
				idDriver_ = 0;
			} else {
				idDriver_ = unit->getId();
			}
		}

		void updatePhysics(float time, float timeStep, Input input) override {
			b2Vec2 force = getDirectionVector();
			
			// Accelate or decelerate
			float throttle = 0.0f;
			if (input.forward_ && !input.backward_) {
				throttle = 20.0;
			} else if (!input.forward_ && input.backward_) {
				throttle = -20.0;
			}
			body_->ApplyForce(throttle*force,getFrontWheelPosition());

			float steering = 0.0f;

			// Turn left or right
			if (input.turnLeft_ && !input.turnRight_) {
				steering = 1.0f;
			} else if (!input.turnLeft_ && input.turnRight_) {
				steering = -1.0f;
			}

			steeringAngle_ = 0.3f*steering;

			applyFriction(2.0f,2.0f,100.0f,100.0f);

			if (input.action_) {
				actionSignal_(this);
			}
		}

		void applyFriction(float frictionForwardFrontWheel, float frictionForwardBackWheel,
			float frictionLateralFrontWheel, float frictionLateralBackWheel) {
			// Back wheel lateral friction.
			b2Vec2 currentRightNormal = body_->GetWorldVector(b2Vec2(0,-1));
			b2Vec2 force = -frictionLateralBackWheel * b2Dot(currentRightNormal, body_->GetLinearVelocityFromWorldPoint(getBackWheelPosition())) * currentRightNormal;
			body_->ApplyForce(force, getBackWheelPosition());

			// Front wheel lateral friction.
			currentRightNormal = b2Vec2(-getDirectionVector().y,getDirectionVector().x);
			force = -frictionLateralFrontWheel * b2Dot(currentRightNormal, body_->GetLinearVelocityFromWorldPoint(getFrontWheelPosition())) * currentRightNormal;
			body_->ApplyForce(force, getFrontWheelPosition());
			
			// Back wheel forward friction.
			force = -frictionForwardBackWheel * b2Dot(getDirectionVector(), body_->GetLinearVelocity()) * getDirectionVector();
			body_->ApplyForce(force, getBackWheelPosition());

			// Front wheel forward friction.
			b2Vec2 forward = body_->GetWorldVector(b2Vec2(1,0));
			force = -frictionForwardFrontWheel * b2Dot(forward, body_->GetLinearVelocity()) * forward;
			body_->ApplyForce(force, getFrontWheelPosition());
		}

		void applySpin(float impulse) {
			body_->ApplyAngularImpulse(impulse);
		}

		State state() const {
			State state;
			state.position_ = body_->GetPosition();
			state.angle_ = body_->GetAngle();
			state.anglularVelocity_ = body_->GetAngularVelocity();
			state.velocity_ = body_->GetLinearVelocity();
			return state;
		}

		b2Vec2 getFrontWheelPosition() const {
			return body_->GetWorldPoint(b2Vec2(length_ * wheelDelta_,0));
		}

		b2Vec2 getBackWheelPosition() const {
			return body_->GetWorldPoint(b2Vec2(-length_ * wheelDelta_,0));
		}
		
		b2Vec2 getDirectionVector() const {
			return body_->GetWorldVector(b2Vec2(std::cos(steeringAngle_),std::sin(steeringAngle_)));
		}
		
		double width() const {
			return width_;
		}

		double length() const {
			return length_;
		}

		bool isInsideViewArea(Position position) const {
			return true;
		}

		Position getPosition() const {
			return Position(body_->GetPosition().x,body_->GetPosition().y);
		}

		b2Body* getBody() const override {
			return body_;
		}

		Weapon getWeapon() const override {
			return Weapon();
		}

		float getDirection() const override {
			return steeringAngle_;
		}

		bool isInfected() const override {
			// When no driver the car is seen as infected and therefore ignored by the zombies.
 			return Object::getObject(idDriver_) == nullptr;
		}

		float getViewDistance() const override {
			return 10;
		}

		bool isDead() const override {
			return false;
		}

		mw::signals::Connection addActionHandler(mw::Signal<Car*>::Callback callback) {
			return actionSignal_.connect(callback);
		}
	private:
		mw::Signal<Car*> actionSignal_;

		b2Body* body_;
		float steeringAngle_;

		int idDriver_;
		float length_, width_;
		State state_;
		float currentTime_;
		float wheelDelta_;
	};

} // Namespace zombie.

#endif // CAR_H