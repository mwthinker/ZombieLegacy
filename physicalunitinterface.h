#ifndef PHYSICALUNITINTERFACE_H
#define PHYSICALUNITINTERFACE_H

#include <mw/mathvector.h>

namespace zombie {

typedef mw::MathVector Velocity;
typedef Velocity Position;
typedef Velocity Force;
typedef Velocity Vec3;

class PhysicalUnitInterface {
public:
	virtual bool toRemove() const = 0;

	virtual ~PhysicalUnitInterface() {
	}
	
	virtual double stiffness() const = 0;
	virtual double damping() const = 0;
	virtual double mass() const = 0;
	virtual Force force() const = 0;
	virtual void addForce(Force force) = 0;
	virtual void resetForce() = 0;
	virtual void setVelocity(Velocity velocity) = 0;
	virtual Velocity getVelocity() const = 0;
	virtual void setPosition(Position position) = 0;
	virtual Position getPosition() const = 0;
	virtual double radius() const = 0;
private:
};

}

#endif PHYSICALUNIT_H