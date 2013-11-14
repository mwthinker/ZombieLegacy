#include "unit.h"
#include "input.h"
#include "weapon.h"
#include "auxiliary.h"

#include <Box2D/Box2D.h>
#include <cmath>

namespace zombie {

	Unit::Unit(b2World* world, const State& state, float mass, float radius, float life, float walkingSpeed, float runningSpeed, bool infected, const Weapon& weapon) : MovingObject(world), weapon_(weapon) {
		isInfected_ = infected;

		// Properties
		viewDistance_ = 10.f;
		viewAngle_ = 120.f/180.f * PI;
		smallViewDistance_ = 2;

		// Health
		healthPoints_ = life;
		isDead_ = false;

		timeLeftToRun_ = 5.f;
		
		// Box2d properties.
		b2BodyDef bodyDef;
		bodyDef.type = b2_dynamicBody;
		bodyDef.position.Set(state.position_.x, state.position_.y);
		bodyDef.angle = state.angle_;

		body_ = getWorld()->CreateBody(&bodyDef);
		body_->SetUserData(this);

		// Add tensor to make all objects inside the tenson visible.
		{
			b2CircleShape circle;
			circle.m_p.Set(0, 0);
			circle.m_radius = viewDistance_;

			b2FixtureDef fixtureDef;
			fixtureDef.shape = &circle;
			fixtureDef.density = 0.0f;
			fixtureDef.friction = 0.0f;
			fixtureDef.isSensor = true;
		
			// Add Body fixture.
			b2Fixture* fixture = body_->CreateFixture(&fixtureDef);
			fixture->SetUserData(nullptr);
		}

		// Add body properties.
		{
			b2CircleShape circle;
			circle.m_p.Set(0, 0);
			circle.m_radius = radius;

			b2FixtureDef fixtureDef;
			fixtureDef.shape = &circle;
			fixtureDef.density = mass / (PI * radius * radius);
			fixtureDef.friction = 0.0f;
		
			// Add Body fixture.
			b2Fixture* fixture = body_->CreateFixture(&fixtureDef);
			fixture->SetUserData(this);
		}
	}

	Unit::~Unit() {
		eventSignal_(this, REMOVED);
		getWorld()->DestroyBody(body_);
	}

	void Unit::updatePhysics(float time, float timeStep, Input input) {
		if (!isDead()) {
			float angle = getDirection();
			Force move = Vec3(std::cos(angle),std::sin(angle));

			// Time left to run?
			if (timeLeftToRun_ >= 0) {
				if (input.forward_ && input.run_) {
					timeLeftToRun_ -= timeStep;
					move *= 2;
					eventSignal_(this, UnitEvent::RUN);
				} else if (timeLeftToRun_ < 5) {
					timeLeftToRun_ += timeStep;
				}
			} else { // Time is negative!
				timeLeftToRun_ += timeStep;
			}

			// Move forward or backwards.
			if (input.forward_ && !input.backward_) {
				body_->ApplyForceToCenter(b2Vec2(move.x,move.y));				
				eventSignal_(this, UnitEvent::WALK);
			} else if (!input.forward_ && input.backward_) {
				body_->ApplyForceToCenter(-b2Vec2(move.x,move.y));
				eventSignal_(this, UnitEvent::WALK);
			} else {
				// In order to make the unit stop when not moving.
				body_->ApplyForceToCenter(-body_->GetLinearVelocity());
				eventSignal_(this, UnitEvent::STANDSTILL);
			}
			
			// Add friction.
			body_->ApplyForceToCenter(-body_->GetLinearVelocity());

			// Turn left or right.
			if (input.turnLeft_ && !input.turnRight_) {				
				body_->SetAngularVelocity(3.0f);
			} else if (!input.turnLeft_ && input.turnRight_) {
				body_->SetAngularVelocity(-3.0f);
			} else {
				body_->SetAngularVelocity(0.0);
			}

			// Want to shoot? And weapon is ready?
			if (input.shoot_ && weapon_.shoot(time)) {
				bullet_.direction_ = angle;
				bullet_.range_ = weapon_.range();
				bullet_.postion_ = getPosition();
				bullet_.damage_ = weapon_.damage();
				eventSignal_(this, UnitEvent::SHOOT);
			}

			// Want to reload? And weapon is ready?
			if (input.reload_ && weapon_.reload()) {
				input.reload_ = true;
				eventSignal_(this, UnitEvent::RELOADING);
			}

			if (input.action_) {
				eventSignal_(this, UnitEvent::ACTION);
			}
		}
	}

	void Unit::collision(float impulse) {
	}

	State Unit::getState() const {
		State state;
		state.angle_ = body_->GetAngle();
		state.position_ = body_->GetPosition();
		state.velocity_ = body_->GetLinearVelocity();
		state.anglularVelocity_ = body_->GetAngularVelocity();
		return state;
	}

	float Unit::getViewDistance() const {
		return viewDistance_;
	}

	float Unit::smallViewDistance() {
		return smallViewDistance_;
	}

	float Unit::viewAngle() const {
		return viewAngle_;
	}

	bool Unit::isInside(Position position) const {
		Position p = getPosition();
		return (position - getPosition()).LengthSquared() < getRadius()*getRadius();
	}

	bool Unit::isInsideViewArea(Position position) const {
		Position p = position - getPosition();
		double angle = std::atan2(p.y, p.x);
		return calculateDifferenceBetweenAngles(angle,body_->GetAngle() + viewAngle() * 0.5) < 0 
			&& calculateDifferenceBetweenAngles(angle,body_->GetAngle() - viewAngle() * 0.5) > 0
			&& p.LengthSquared() < getViewDistance() * getViewDistance() || isInsideSmalViewDistance(position);
	}

	bool Unit::isInsideSmalViewDistance(Position position) const {
		return (position - getPosition()).LengthSquared() < smallViewDistance_*smallViewDistance_;
	}	

	float Unit::getDirection() const {
		return body_->GetAngle();
	}

	mw::signals::Connection Unit::addEventHandler(mw::Signal<Unit*, UnitEvent>::Callback callback) {
		return eventSignal_.connect(callback);
	}

	float Unit::healthPoints() const {
		return healthPoints_;
	}

	void Unit::updateHealthPoint(float deltaLife) {
		if (!isDead_) {
			healthPoints_ += deltaLife;
		}
		if (healthPoints_ < 0) {
			isDead_ = true;
		}
	}

	bool Unit::isDead() const {
		return isDead_;
	}

} // Namespace zombie.
