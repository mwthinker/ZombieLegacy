#include "physicalunit.h"

#include "protocol.h"
#include "object.h"

#include <mw/mathvector.h>
#include <mw/packet.h>

namespace zombie {

	PhysicalUnit::PhysicalUnit(int id, double x, double y, double radius, double stiffness, double damping, double mass) : Object(id) {
		position_ = Position(x,y);
		radius_ = radius;
		stiffness_ = stiffness;
		damping_ = damping;
		mass_ = mass;
	}

	PhysicalUnit::PhysicalUnit(mw::Packet& packet) : Object(packet) {
		packet >> position_;
		packet >> radius_;
		packet >> stiffness_;
		packet >> damping_;
		packet >> mass_;
	}

	mw::Packet PhysicalUnit::generatePacket() const {
		mw::Packet packet;
		packet << Object::generatePacket();
		packet << position_;
		packet << radius_;
		packet << stiffness_;
		packet << damping_;
		packet << mass_;
		return packet;
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
