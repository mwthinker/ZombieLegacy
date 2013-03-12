#include "unit.h"

#include "object.h"
#include "input.h"
#include "weapon.h"
#include "auxiliary.h"

#include <mw/mathvector.h>
#include <Box2D/Box2D.h>

#include <cmath>
#include <memory>

namespace zombie {

	Unit::Unit(float x, float y, float angle, Weapon weapon, bool infected) : weapon_(weapon) {		
		//PhysicalUnit(x,y,0.4, 50.0,0.0,10.0)
		b2BodyDef bodyDef;
		bodyDef.type = b2_dynamicBody;
		bodyDef.position.Set(x, y);
		bodyDef.angle = angle;
		bodyDef.fixedRotation = true;
		body_ = getWorld()->CreateBody(&bodyDef);

		b2CircleShape circle;
		circle.m_p.Set(0, 0);
		circle.m_radius = 0.4f;

		b2FixtureDef fixtureDef;
		fixtureDef.shape = &circle;
		fixtureDef.density = 1.0f;
		fixtureDef.friction = 0.3f;
		b2Fixture* fixture = body_->CreateFixture(&fixtureDef);
		fixture->SetUserData(this);
		//fixture->SetUserData(this);
		
		angleVelocity_ = 0.0f;

		isInfected_ = infected;

		// Properties
		viewDistance_ = 10.f;
		viewAngle_ = 120/180.0*b2_pi;
		smallViewDistance_ = 2;

		// Unit's direction Angle.
		angle_ = angle;

		// Health
		healthPoints_ = 100.0f;
		isDead_ = false;
		
		timeLeftToRun_ = 5.f;
	}

	Unit::~Unit() {
		getWorld()->DestroyBody(body_);
	}

	void Unit::updatePhysics(float time, float timeStep, Input input) {
		if (!isDead()) {
			float angle = moveDirection();	

			Force move = Vec3(std::cos(angle),std::sin(angle));

			// Time left to run?
			if (timeLeftToRun_ >= 0) {
				if (input.forward_ && input.run_) {
					timeLeftToRun_ -= timeStep;
					move *= 2;
					sendEventToHandlers(UnitEvent::RUN);
				} else if (timeLeftToRun_ < 5) {
					timeLeftToRun_ += timeStep;
				}
			} else { // Time is negative!
				timeLeftToRun_ += timeStep;
			}

			// Move forward or backwards.
			if (input.forward_ && !input.backward_) {
				body_->ApplyForceToCenter(b2Vec2(move.x,move.y));
				//addForce(move);
				sendEventToHandlers(UnitEvent::WALK);
			} else if (!input.forward_ && input.backward_) {
				body_->ApplyForceToCenter(-b2Vec2(move.x,move.y));
				//addForce(-move);
				sendEventToHandlers(UnitEvent::WALK);
			} else {
				// In order to make the unit stop when not moving.
				//body_->ApplyForceToCenter(-body_->GetLinearVelocity());
				//addForce(-getVelocity()*5);
				sendEventToHandlers(UnitEvent::STANDSTILL);
			}

			//std::cout << getState().position_ << std::endl;

			// Turn left or right.
			if (input.turnLeft_ && !input.turnRight_) {
				turn(3*timeStep);
			} else if (!input.turnLeft_ && input.turnRight_) {
				turn(-3*timeStep);
			}

			// Want to shoot? And weapon is ready?
			if (input.shoot_  && weapon_.shoot(time)) {
				Bullet bullet;
				bullet.direction_ = angle;
				bullet.range_ = weapon_.range();
				bullet.postion_ = getPosition();
				bullet.damage_ = weapon_.damage();
				bullets_.push(bullet);
			
				sendEventToHandlers(UnitEvent::SHOOT);
			}

			// Want to reload? And weapon is ready?
			if (input.reload_ && weapon_.reload()) {
				input.reload_ = true;
				sendEventToHandlers(UnitEvent::RELOADING);
			}
		}
	}

	void Unit::sendEventToHandlers(UnitEvent unitEvent) const {
		for (auto handler : eventHandlers_) {
			handler(unitEvent);
		}
	}

	void Unit::setState(State state) {
		angle_ = state.angle_;
		//setPosition(state.position_);
		//setVelocity(state.velocity_);
		angleVelocity_ = state.angleVelocity_;
	}

	State Unit::getState() const {
		State state;
		state.angle_ = angle_;
		state.position_ = Position(body_->GetPosition().x,body_->GetPosition().y);
		state.velocity_ = Position(body_->GetLinearVelocity().x,body_->GetLinearVelocity().y);
		state.angleVelocity_ = angleVelocity_;
		return state;
	}

	float Unit::viewDistance() {
		return viewDistance_;
	}

	float Unit::smallViewDistance() {
		return smallViewDistance_;
	}

	float Unit::viewAngle() const {
		return viewAngle_;
	}

	bool Unit::isInside(float x, float y) const {
		Position p = getPosition();
		return (x - p.x)*(x - p.x) + (y - p.y)*(y - p.y) < radius()*radius();
	}

	bool Unit::isInsideSmalViewDistance(float x, float y) const {
		Position p = Position(x,y) - getPosition();
		return p.LengthSquared() < smallViewDistance_*smallViewDistance_;
	}

	bool Unit::isPointViewable(float x, float y) {
		Position p = Position(x,y) - getPosition();
		double angle = std::atan2(p.y, p.x);
		return calculateDifferenceBetweenAngles(angle,angle_ + viewAngle() * 0.5) < 0 
			&& calculateDifferenceBetweenAngles(angle,angle_ - viewAngle() * 0.5) > 0
			&& p.LengthSquared() < viewDistance() * viewDistance();
	}

	float Unit::moveDirection() const {
		return angle_;
	}

	bool Unit::pollShot(Bullet& bullet) {
		if (bullets_.empty()) {
			return false;
		}

		bullet = bullets_.front();
		bullets_.pop();

		return true;
	}

	void Unit::addEventHandler(std::function<void(UnitEvent)> handler) {
		eventHandlers_.push_back(handler);
	}

	void Unit::turn(float angle) {
		angle_ += angle;

		if (angle_ > b2_pi) {
			angle_ -= 2*b2_pi;
		} else if (angle_ < -b2_pi) {
			angle_ += 2*b2_pi;
		}
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

} // namespace zombie.