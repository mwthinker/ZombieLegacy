#include "unit.h"
#include "input.h"
#include "weapon.h"
#include "auxiliary.h"

#include <cmath>

namespace zombie {

	Unit::Unit(float mass, float radius, float life,
		float walkingSpeed, float runningSpeed, bool infected,
		Position grip, const WeaponPtr& weapon) : weapon_(weapon) {

		isInfected_ = infected;

		// Properties
		viewDistance_ = 10.f;
		viewAngle_ = 120.f / 180.f * PI;
		smallViewDistance_ = 2;

		// Health
		healthPoints_ = life;
		isDead_ = false;
		died_ = false;

		timeLeftToRun_ = 5.f;

		radius_ = radius;
		mass_ = mass;
		grip_ = grip;

		body_ = nullptr;
	}

	Unit::Unit(const Unit& unit) {
		weapon_ = unit.weapon_->clone();
		isInfected_ = unit.isInfected_;

		// Properties
		viewDistance_ = unit.viewAngle_;
		viewAngle_ = unit.viewAngle_;
		smallViewDistance_ = unit.smallViewDistance_;

		// Health
		healthPoints_ = unit.healthPoints_;
		isDead_ = unit.isDead_;
		died_ = unit.died_;

		timeLeftToRun_ = unit.timeLeftToRun_;

		radius_ = unit.radius_;
		mass_ = unit.mass_;
		grip_ = unit.grip_;

		body_ = nullptr;
	}

	Unit& Unit::operator=(const Unit& unit) {
		weapon_ = unit.weapon_->clone();
		isInfected_ = unit.isInfected_;

		// Properties
		viewDistance_ = unit.viewAngle_;
		viewAngle_ = unit.viewAngle_;
		smallViewDistance_ = unit.smallViewDistance_;

		// Health
		healthPoints_ = unit.healthPoints_;
		isDead_ = unit.isDead_;
		died_ = unit.died_;

		timeLeftToRun_ = unit.timeLeftToRun_;

		radius_ = unit.radius_;
		mass_ = unit.mass_;
		grip_ = unit.grip_;

		body_ = nullptr;

		return *this;
	}

	void Unit::createBody(b2World* world) {
		// Box2d properties.
		b2BodyDef bodyDef;
		bodyDef.type = b2_dynamicBody;
		bodyDef.position = ZERO;// tate.position_;
		bodyDef.angle = 0;// state.angle_;

		body_ = world->CreateBody(&bodyDef);
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
			circle.m_radius = radius_;

			b2FixtureDef fixtureDef;
			fixtureDef.shape = &circle;
			fixtureDef.density = mass_ / (PI * radius_ * radius_);
			fixtureDef.friction = 0.0f;

			// Add Body fixture.
			b2Fixture* fixture = body_->CreateFixture(&fixtureDef);
			fixture->SetUserData(this);
		}
	}

	Unit::~Unit() {
	}

	// All UnitEvent is only allowed to be triggered in this function.
	// This in order to avoid adjusting box2d valuse during a box2d step, which
	// would cause undefined behavior.
	void Unit::updatePhysics(float time, float timeStep) {
		if (died_) {
			signal(DIE);
			isDead_ = true;
		}

		if (!isDead()) {
			Input input = getInput();
			float angle = getDirection();
			Force move = Vec3(std::cos(angle), std::sin(angle));

			// Time left to run?
			if (timeLeftToRun_ >= 0) {
				if (input.forward_ && input.run_) {
					timeLeftToRun_ -= timeStep;
					move *= 2;
					signal(RUN);
				} else if (timeLeftToRun_ < 5) {
					timeLeftToRun_ += timeStep;
				}
			} else { // Time is negative!
				timeLeftToRun_ += timeStep;
			}

			// Move forward or backwards.
			if (input.forward_ && !input.backward_) {
				body_->ApplyForceToCenter(b2Vec2(move.x, move.y));
				signal(WALK);
			} else if (!input.forward_ && input.backward_) {
				body_->ApplyForceToCenter(-b2Vec2(move.x, move.y));
				signal(WALK);
			} else {
				// In order to make the unit stop when not moving.
				body_->ApplyForceToCenter(-body_->GetLinearVelocity());
				signal(STANDSTILL);
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

			// Want to shoot?
			if (input.shoot_) {
				if (weapon_) {
					weapon_->pullTrigger(*this, time);
					weapon_->releaseTrigger(*this, time);
				}
			}

			// Want to reload?
			if (input.reload_) {
				if (weapon_) {
					weapon_->reload(time);
				}
			}

			if (input.action_) {
				signal(ACTION);
			}
		}
	}
	
	State Unit::getState() const {
		return State(body_->GetPosition(),
					 body_->GetLinearVelocity(),
					 body_->GetAngle(),
					 body_->GetAngularVelocity());
	}

	void Unit::setState(const State& state) {
		// Set the position and current angle.
		body_->SetTransform(state.position_ - body_->GetPosition(), state.angle_ - body_->GetAngle());
		
		// Set the velocity of the states.
		body_->SetAngularVelocity(state.anglularVelocity_);
		body_->SetLinearVelocity(body_->GetLinearVelocity());
	}

	float Unit::getViewDistance() const {
		return viewDistance_;
	}

	float Unit::smallViewDistance() const {
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
		return calculateDifferenceBetweenAngles(angle, body_->GetAngle() + viewAngle() * 0.5) < 0
			&& calculateDifferenceBetweenAngles(angle, body_->GetAngle() - viewAngle() * 0.5) > 0
			&& p.LengthSquared() < getViewDistance() * getViewDistance() || isInsideSmalViewDistance(position);
	}

	bool Unit::isInsideSmalViewDistance(Position position) const {
		return (position - getPosition()).LengthSquared() < smallViewDistance_*smallViewDistance_;
	}

	float Unit::getDirection() const {
		return body_->GetAngle();
	}

	float Unit::healthPoints() const {
		return healthPoints_;
	}

	void Unit::updateHealthPoint(float deltaLife) {
		if (!isDead_) {
			healthPoints_ += deltaLife;
			signal(INJURED);
		}
		if (healthPoints_ < 0) {
			died_ = true;
		}
	}

	bool Unit::isDead() const {
		return isDead_;
	}

	Position Unit::getPosition() const {
		return body_->GetPosition();
	}

	float Unit::getRadius() const {
		return radius_;
	}

	b2Body* Unit::getBody() const {
		return body_;
	}

	void Unit::signal(int eventType) {
		getActionHandler()->unitEvent(this, eventType);
		eventHandler(eventType);
	}

} // Namespace zombie.
