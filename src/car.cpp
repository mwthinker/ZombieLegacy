#include "car.h"
#include "movingobject.h"
#include "input.h"
#include "state.h"
#include "unit.h"
#include "driver.h"
#include "unit.h"
#include "building.h"

#include <cmath>

namespace zombie {

	Car::Car(b2World* world, const State& state, float mass, float life, float width, float length) : MovingObject(world) {
		length_ = length;
		width_ = width;

		currentTime_ = 0.0f;
		steeringAngle_ = 0.0f;
		wheelDelta_ = 0.4f;

		// Box2d properties.
		b2BodyDef bodyDef;
		bodyDef.type = b2_dynamicBody;
		bodyDef.position.Set(state.position_.x, state.position_.y);
		bodyDef.angle = state.angle_;
		body_ = getWorld()->CreateBody(&bodyDef);
		body_->SetUserData(this);

		// Body properties.
		{
			b2PolygonShape dynamicBox;
			dynamicBox.SetAsBox(0.5f *length_, 0.5f * width_); // Expected parameters is half the side.

			b2FixtureDef fixtureDef;
			fixtureDef.shape = &dynamicBox;
			fixtureDef.density = mass / (length_ * width_);
			fixtureDef.friction = 0.3f;
			b2Fixture* fixture = body_->CreateFixture(&fixtureDef);
			fixture->SetUserData(this);
		}
	}

	Car::~Car() {
		eventSignal_(this, REMOVED);
		getWorld()->DestroyBody(body_);
	}

	Driver* Car::getDriver() const {
		return static_cast<Driver*>(player_);
	}

	void Car::setDriver(Driver* driver) {
		player_ = driver;
	}

	void Car::updatePhysics(float time, float timeStep, Input input) {
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

		steeringAngle_ = 0.3f * steering;

		applyFriction(2.0f, 2.0f, 100.0f, 100.0f);

		eventSignal_(this, MOVED);

		if (input.action_) {
			eventSignal_(this, ACTION);
		}
	}

	State Car::getState() const {
		State state;
		state.position_ = body_->GetPosition();
		state.angle_ = body_->GetAngle();
		state.anglularVelocity_ = body_->GetAngularVelocity();
		state.velocity_ = body_->GetLinearVelocity();
		return state;
	}

	bool Car::isInsideViewArea(Position position) const {
		return true;
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

	void Car::collisionWith(Unit* unit, float impulse) {
	}

	void Car::collisionWith(Car* car, float impulse) {
	}

	void Car::collisionWith(Building* building, float impulses) {
	}

} // Namespace zombie.
