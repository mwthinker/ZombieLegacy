#include "unit.h"

#include "object.h"
#include "input.h"
#include "weapon.h"
#include "physicalunit.h"
#include "auxiliary.h"

#include <mw/mathvector.h>

#include <cmath>
#include <memory>

namespace zombie {

	Unit::Unit(double x, double y, double angle, Weapon weapon, bool infected, int id) : PhysicalUnit(id, x,y,0.4, 50.0,0.0,10.0), weapon_(weapon) {		
		angleVelocity_ = 0.0;

		isInfected_ = infected;

		// Properties
		viewDistance_ = 10.0;
		viewAngle_ = 120.0/180.0*mw::PI;
		smallViewDistance_ = 2;

		// Unit's direction Angle.
		angle_ = angle;

		// Health
		healthPoints_ = 100.0;
		isDead_ = false;
		
		timeLeftToRun_ = 5;
	}

	Unit::~Unit() {
	}

	bool Unit::toRemove() const {
		return isDead();
	}

	void Unit::updatePhysics(double time, double timeStep, Input input) {
		double angle = moveDirection();	

		Force move = Vec3(std::cos(angle),std::sin(angle))*30;

		// Time left to run?
		if (timeLeftToRun_ >= 0) {
			if (input.forward_ && input.run_) {
				timeLeftToRun_ -= timeStep;
				move *= 2;
			} else if (timeLeftToRun_ < 5) {
				timeLeftToRun_ += timeStep;
			}
		} else { // Time is negative!
			timeLeftToRun_ += timeStep;
		}

		// Move forward or backwards.
		if (input.forward_ && !input.backward_) {
			addForce(move);
		} else if (!input.forward_ && input.backward_) {
			addForce(-move);
		} else {
			// In order to make the unit stop when not moving.
			addForce(-getVelocity()*5);
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

	void Unit::sendEventToHandlers(UnitEvent unitEvent) const {
		for (auto handler : eventHandlers_) {
			handler(unitEvent);
		}
	}

	void Unit::setState(State state) {
		angle_ = state.angle_;
		setPosition(state.position_);
		setVelocity(state.velocity_);
		angleVelocity_ = state.angleVelocity_;
	}

	State Unit::getState() const {
		State state;
		state.angle_ = angle_;
		state.position_ = getPosition();
		state.velocity_ = getVelocity();
		state.angleVelocity_ = angleVelocity_;
		return state;
	}

	double Unit::viewDistance() {
		return viewDistance_;
	}

	double Unit::smallViewDistance() {
		return smallViewDistance_;
	}

	double Unit::viewAngle() const {
		return viewAngle_;
	}

	bool Unit::isInside(double x, double y) const {
		Position p = getPosition();
		return (x - p.x_)*(x - p.x_) + (y - p.y_)*(y - p.y_) < radius()*radius();
	}

	bool Unit::isInsideSmalViewDistance(double x, double y) const {
		Position p = Position(x,y) - getPosition();
		return p.magnitudeSquared() < smallViewDistance_*smallViewDistance_;
	}

	bool Unit::isPointViewable(double x, double y) {
		Position p = Position(x,y) - getPosition();
		double angle = std::atan2(p.y_, p.x_);
		return calculateDifferenceBetweenAngles(angle,angle_ + viewAngle() * 0.5) < 0 
			&& calculateDifferenceBetweenAngles(angle,angle_ - viewAngle() * 0.5) > 0
			&& p.magnitudeSquared() < viewDistance() * viewDistance();
	}

	double Unit::moveDirection() const {
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

	void Unit::turn(double angle) {
		angle_ += angle;

		if (angle_ > mw::PI) {
			angle_ -= 2*mw::PI;
		} else if (angle_ < -mw::PI) {
			angle_ += 2*mw::PI;
		}
	}

	double Unit::healthPoints() const {
		return healthPoints_;
	}

	void Unit::updateHealthPoint(double deltaLife) {
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