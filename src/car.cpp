#include "car.h"
#include "input.h"
#include "state.h"
#include "unit.h"

#include <cmath>

namespace zombie {

	Car::Car(float mass, float life, float width, float length) {
		body_ = nullptr;
		length_ = length;
		width_ = width;

		currentTime_ = 0.0f;
		steeringAngle_ = 0.0f;
		wheelDelta_ = 0.4f;
		mass_ = mass;
		driver_ = nullptr;
	}

	Car::~Car() {
	}

	Car::Car(const Car& car) {
		length_ = car.length_;
		width_ = car.width_;

		currentTime_ = car.currentTime_;
		steeringAngle_ = car.steeringAngle_;
		wheelDelta_ = car.wheelDelta_;
		mass_ = car.mass_;
		body_ = nullptr;
	}
	
	Car& Car::operator=(const Car& car) {
		length_ = car.length_;
		width_ = car.width_;

		currentTime_ = car.currentTime_;
		steeringAngle_ = car.steeringAngle_;
		wheelDelta_ = car.wheelDelta_;
		mass_ = car.mass_;
		body_ = nullptr;

		return  *this;
	}

	Unit* Car::getDriver() const {
		return driver_;
	}

	void Car::setDriver(Unit* driver) {
		driver_ = driver;
	}

	void Car::updatePhysics(float time, float timeStep) {
		Input input;
		if (driver_ != nullptr) {
			input = driver_->getInput();
		}
		previousState_ = getState();
		b2Vec2 force = getDirectionVector();

		// Accelate or decelerate
		float throttle = 0.0f;
		if (input.forward_ && !input.backward_) {
			throttle = 20.0;
		} else if (!input.forward_ && input.backward_) {
			throttle = -20.0;
		}
		body_->ApplyForce(throttle*force, getFrontWheelPosition());

		float steering = 0.0f;

		// Turn left or right
		if (input.turnLeft_ && !input.turnRight_) {
			steering = 1.0f;
		} else if (!input.turnLeft_ && input.turnRight_) {
			steering = -1.0f;
		}

		steeringAngle_ = 0.4f * steering;

		applyFriction(2.0f, 2.0f, 100.0f, 100.0f);

		signal(MOVED);

		if (input.action_) {
			signal(ACTION);
		}
	}

	State Car::getState() const {
		return State(body_->GetPosition(),
					 body_->GetLinearVelocity(),
					 body_->GetAngle(),
					 body_->GetAngularVelocity());
	}

	void Car::setState(const State& state) {
		// Set the position and current angle.
		body_->SetTransform(state.position_ - body_->GetPosition(), state.angle_ - body_->GetAngle());

		// Set the velocity of the states.
		body_->SetAngularVelocity(state.anglularVelocity_);
		body_->SetLinearVelocity(body_->GetLinearVelocity());
	}

	bool Car::isInsideViewArea(Position position) const {
		return true;
	}

	void Car::createBody(b2World* world) {
		// Box2d properties.
		b2BodyDef bodyDef;
		bodyDef.type = b2_dynamicBody;
		bodyDef.position = ZERO;//state.position_;
		bodyDef.angle = 0;// state.angle_;
		body_ = world->CreateBody(&bodyDef);
		body_->SetUserData(this);

		// Body properties.
		{
			b2PolygonShape dynamicBox;
			dynamicBox.SetAsBox(0.5f *length_, 0.5f * width_); // Expected parameters is half the side.

			b2FixtureDef fixtureDef;
			fixtureDef.shape = &dynamicBox;
			fixtureDef.density = mass_ / (length_ * width_);
			fixtureDef.friction = 0.3f;
			b2Fixture* fixture = body_->CreateFixture(&fixtureDef);
			fixture->SetUserData(this);
		}
	}

	void Car::applyFriction(float frictionForwardFrontWheel, float frictionForwardBackWheel,
		float frictionLateralFrontWheel, float frictionLateralBackWheel) {
		// Back wheel lateral friction.
		b2Vec2 currentRightNormal = body_->GetWorldVector(b2Vec2(0, -1));
		b2Vec2 force = -frictionLateralBackWheel * b2Dot(currentRightNormal, body_->GetLinearVelocityFromWorldPoint(getBackWheelPosition())) * currentRightNormal;
		body_->ApplyForce(force, getBackWheelPosition());

		// Front wheel lateral friction.
		currentRightNormal = b2Vec2(-getDirectionVector().y, getDirectionVector().x);
		force = -frictionLateralFrontWheel * b2Dot(currentRightNormal, body_->GetLinearVelocityFromWorldPoint(getFrontWheelPosition())) * currentRightNormal;
		body_->ApplyForce(force, getFrontWheelPosition());

		// Back wheel forward friction.
		force = -frictionForwardBackWheel * b2Dot(getDirectionVector(), body_->GetLinearVelocity()) * getDirectionVector();
		body_->ApplyForce(force, getBackWheelPosition());

		// Front wheel forward friction.
		b2Vec2 forward = body_->GetWorldVector(b2Vec2(1, 0));
		force = -frictionForwardFrontWheel * b2Dot(forward, body_->GetLinearVelocity()) * forward;
		body_->ApplyForce(force, getFrontWheelPosition());
	}

	void Car::signal(int eventType) {
		getActionHandler()->carEvent(this, eventType);
		eventHandler(eventType);
	}

} // Namespace zombie.
