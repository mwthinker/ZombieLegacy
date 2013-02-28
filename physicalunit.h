#ifndef PHYSICALUNIT_H
#define PHYSICALUNIT_H

#include "typedefs.h"

#include "physicalunitinterface.h"
#include "object.h"

namespace zombie {

class PhysicalUnit : public Object, public PhysicalUnitInterface {
public:
	PhysicalUnit(double x, double y, double radius, double stiffness, double damping, double mass);

	// Override member from class PhysicalUnit
	double stiffness() const override;

	// Override member from class PhysicalUnit
	double damping() const override;
	
	// Override member from class PhysicalUnit
	double mass() const override;
		
	Force force() const override;	
	void addForce(Force force) override;
	void resetForce() override;

	void setVelocity(Velocity velocity) override;
	Velocity getVelocity() const override;		
	void setPosition(Position position) override;
	Position getPosition() const override;	

	double radius() const override;
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