#ifndef CAR_H
#define CAR_H

#include "object.h"
#include "input.h"
#include "state.h"

#include <Box2D/Box2D.h>
#include <mw/mathvector.h>

#include <cmath>
#include <functional>

namespace zombie {

	enum CarEvent {CAREVENT_EXPLODE, CAREVENT_ACCELERATE, CAREVENT_BRAKE};

	class Car : public Object {
	public:
		Car(float x, float y, float angle = 0.0) {
			length_ = 2.0f;
			width_ = 1.0f;
			float mass = 100.0f;

			b2BodyDef bodyDef;
			bodyDef.type = b2_dynamicBody;
			bodyDef.position.Set(x, y);
			bodyDef.angle = angle;
			body_ = getWorld()->CreateBody(&bodyDef);
			
			b2PolygonShape dynamicBox;
			dynamicBox.SetAsBox(length_, width_);

			b2FixtureDef fixtureDef;
			fixtureDef.shape = &dynamicBox;
			fixtureDef.density = 1.0f;
			fixtureDef.friction = 0.3f;
			b2Fixture* fixture = body_->CreateFixture(&fixtureDef);
			fixture->SetUserData(this);

			currentTime_ = 0.0f;
			steeringAngle_ = 0.0f;
			wheelDelta_ = 0.4f;			
		}

		~Car() {
			getWorld()->DestroyBody(body_);
		}

		void updatePhysics(float time, float timeStep, Input input) {
			b2Vec2 force = getDirection();
			//std::cout << "Angle: " << body_->GetAngle() << std::endl;
			//std::cout << "MassC: " << body_->GetWorldCenter().x << " " << body_->GetWorldCenter().y << std::endl;
			
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

			/*
			b2RayCastOutput* rOut;
			b2RayCastInput rIn;			
			rIn.maxFraction = 5;
			rIn.p1 = getBackWheelPosition();
			rIn.p2 = getFrontWheelPosition();
			*/

			if (input.shoot_) {
				if (callbackShoot_ != 0) {
					callbackShoot_(this);
				}
			}

			steeringAngle_ = 0.3f*steering;
		}

		void applyFriction(float frictionForwardFrontWheel, float frictionForwardBackWheel,
			float frictionLateralFrontWheel, float frictionLateralBackWheel) {
			// Back wheel lateral friction.
			b2Vec2 currentRightNormal = body_->GetWorldVector(b2Vec2(0,-1));
			b2Vec2 force = -frictionLateralBackWheel * b2Dot(currentRightNormal, body_->GetLinearVelocityFromWorldPoint(getBackWheelPosition())) * currentRightNormal;
			body_->ApplyForce(force,getBackWheelPosition());			
			//std::cout << "MassC: " << force.Length() << std::endl;

			// Front wheel lateral friction.			
			currentRightNormal = b2Vec2(-getDirection().y,getDirection().x);
			force = -frictionLateralFrontWheel * b2Dot(currentRightNormal, body_->GetLinearVelocityFromWorldPoint(getFrontWheelPosition())) * currentRightNormal;
			body_->ApplyForce(force,getFrontWheelPosition());
			
			// Back wheel forward friction.
			force = -frictionForwardBackWheel * b2Dot(getDirection(), body_->GetLinearVelocity()) * getDirection();
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

		b2Vec2 getDirection() const {
			return body_->GetWorldVector(b2Vec2(std::cos(steeringAngle_),std::sin(steeringAngle_)));
		}

		double width() const {
			return width_;
		}

		double length() const {
			return length_;
		}

		b2Body* getBody() const {
			return body_;
		}

		void setShootCallback(std::function<void(Car*)> callbackShoot) {
			callbackShoot_ = callbackShoot;
		}
	private:
		std::function<void(Car*)> callbackShoot_;

		b2Body* body_;
		float steeringAngle_;

		Input currentInput_;

		float length_, width_;
		State state_;
		float currentTime_;
		float wheelDelta_;
	};

} // Namespace zombie.



#endif // CAR_H