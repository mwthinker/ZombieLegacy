#ifndef PHYSICALUNIT_H
#define PHYSICALUNIT_H

#include <mw/mathvector.h>
#include <mw/packet.h>

#include "physicalunitinterface.h"
#include "object.h"

namespace zombie {

class PhysicalUnit : public Object, public PhysicalUnitInterface {
public:
	PhysicalUnit(int id, double x, double y, double radius, double stiffness, double damping, double mass);
	PhysicalUnit(mw::Packet& packet);
	virtual mw::Packet generatePacket() const;

	// Override member from class PhysicalUnit
	double stiffness() const;

	// Override member from class PhysicalUnit
	double damping() const;
	
	// Override member from class PhysicalUnit
	double mass() const;
		
	Force force() const;	
	void addForce(Force force);
	void resetForce();

	void setVelocity(Velocity velocity);
	Velocity getVelocity() const;		
	void setPosition(Position position);
	Position getPosition() const;	

	double radius() const;
private:
	double stiffness_;
	double damping_;
	double mass_;

	Force force_;
	Velocity velocity_;
	Position position_;

	double radius_;
};

}

#endif PHYSICALUNIT_H