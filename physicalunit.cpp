#include "physicalunit.h"

#include "object.h"

#include <mw/mathvector.h>

namespace zombie {

	PhysicalUnit::PhysicalUnit(int id, double x, double y, double radius, double stiffness, double damping, double mass) : Object(id) {
		position_ = Position(x,y);
		radius_ = radius;
		stiffness_ = stiffness;
		damping_ = damping;
		mass_ = mass;
	}

	double PhysicalUnit::stiffness() const {
		return stiffness_;
	}

	// Override member from class PhysicalUnit
	double PhysicalUnit::damping() const {
		return damping_;
	}
	
	// Override member from class PhysicalUnit
	double PhysicalUnit::mass() const {
		return mass_;
	}
		
	Force PhysicalUnit::force() const {
		return force_;
	}
	
	void PhysicalUnit::addForce(Force force) {
		force_ += force;
	}

	void PhysicalUnit::resetForce() {
		force_ = Force();
	}

	void PhysicalUnit::setVelocity(Velocity velocity) {
		velocity_ = velocity;
	}

	Velocity PhysicalUnit::getVelocity() const {
		return velocity_;
	}
		
	void PhysicalUnit::setPosition(Position position) {
		position_ = position;
	}

	Position PhysicalUnit::getPosition() const {
		return position_;
	}
	
	double PhysicalUnit::radius() const {
		return radius_;
	}
}
