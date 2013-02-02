#ifndef PHYSICALENGINE_H
#define PHYSICALENGINE_H

#include <mw/mathvector.h>

#include <list>
#include <utility>
#include <algorithm>

#include "unit.h"
#include "physicalunit.h"

namespace zombie {

class StaticPhyscalUnit {
public:
	// Calculates if a circle, in position (x,y) with radius (radius), is inside this unit.
	// The operation should be cheap to perform and is only a rough approximation. 
	// It should guarantees that a false response is the same as the circle is outside.
	// However, true response is not a guarantee that the circle is inside only an suggestion that
	// further tests should be done.
	virtual bool isInsideApproximate(double x, double y, double radius) const = 0;
	
	// Returns the stiffness of the unit.
	virtual double stiffness() const = 0;

	// Highly costly operation. Returns the penetration vector of the circle inside the unit.

	// The vector returned is the shortest vector of all possible vectors, that is directed 
	// from the circle center and only exist inside this unit. I.e. the returned penetration 
	// vector is directed towards the figure. If no penetration occurs the zero vector 
	// is returned.
	virtual Vec3 penetration(double x, double y, double radius) const = 0;
};

typedef std::shared_ptr<PhysicalUnitInterface> PhysicalUnitPtr;
typedef std::shared_ptr<StaticPhyscalUnit> StaticPhyscalUnitPtr;

bool inline inside(const PhysicalUnitPtr& unit1, const PhysicalUnitPtr& unit2) {
	return (unit1->getPosition() - unit2->getPosition())*(unit1->getPosition() - unit2->getPosition()) < (unit1->radius() + unit2->radius()) * (unit1->radius() + unit2->radius());	
}

class PhysicalEngine {
public:
	PhysicalEngine() {
	}

	~PhysicalEngine() {
	}

	/* Adds a unit that is effected by the physics.	*/
	void add(const PhysicalUnitPtr& unit) {
		units_.push_back(PhUnit(unit,Vec3()));
	}

	/* Adds a static unit that only effect non static units. */
	void add(StaticPhyscalUnitPtr unit) {
		staticUnits_.push_back(unit);
	}

	/* Removes the corresspondig unit. */
	void remove(const PhysicalUnitPtr& unit) {		
		units_.remove(PhUnit(unit,Vec3()));
	}

	/* Removes the corresspondig static unit. */
	void remove(StaticPhyscalUnitPtr unit) {
		staticUnits_.remove(unit);
	}

	/* Simulates the physics for all units. The physics is simulated one time step. */
	void update(double timeStep) {
		for (PhUnitList::iterator it1 = units_.begin(); it1 != units_.end(); ++it1) {			
			PhUnit& u1 = *it1;			
			for (PhUnitList::iterator it2 = units_.begin(); it2 != units_.end(); ++it2) {
				PhUnit& u2 = *it2;
				if (u1.first != u2.first && inside(u1.first,u2.first)) {
					addForceDueToCollision(u1,u2);
				}
			}
			
			for (StaticPhUnitList::iterator it3 = staticUnits_.begin(); it3 != staticUnits_.end(); ++it3) {
				StaticPhyscalUnitPtr& staticUnit = *it3;
				Vec3 p = u1.first->getPosition();
				if (staticUnit->isInsideApproximate(p.x_,p.y_,u1.first->radius())) {
					addForceDueToCollision(u1,staticUnit);
				}
			}
		}		
		
		for (PhUnitList::iterator it = units_.begin(); it != units_.end(); ++it) {
			PhUnit& unit = *it;
			addViscosity(unit);
			addFriction(unit);
		}

		for (PhUnitList::iterator it = units_.begin(); it != units_.end(); ++it) {
			PhUnit& unit = *it;
			updateUnit(unit,timeStep);
		}
	}
	
private:	
	typedef std::list<StaticPhyscalUnitPtr> StaticPhUnitList;
	typedef std::pair<PhysicalUnitPtr, Vec3> PhUnit;
	typedef std::list<PhUnit> PhUnitList;
	
	friend bool operator==(const PhUnit&, const PhUnit&);

	void addViscosity(PhUnit& u) {
		double speedSquared = u.first->getVelocity()*u.first->getVelocity();
		if (speedSquared > 0.00001*0.00001) { // In order to avoid dividing with zero when normallizing the velocity vector.			
			u.second += -10*speedSquared*u.first->getVelocity().normalize();
		}
	}

	void addFriction(PhUnit& u) {
		double c = 0.10;
		Vec3 velocity = u.first->getVelocity();
		if (velocity.magnitude() > 0.0001) {
			Vec3 force = -c * u.first->mass()*velocity.normalize();
			u.second += force;
		}
	}

	Vec3 penetration(const PhysicalUnitPtr& u1, const PhysicalUnitPtr& u2) {
		Vec3 diff = (u2->getPosition() - u1->getPosition());
		return diff+diff.normalize()*(u2->radius() - u1->radius());
	}

	void addForceDueToCollision(PhUnit& u1, PhUnit& u2) {
		Vec3 penetrationVector1 = penetration(u1.first,u2.first);
		Vec3 penetrationVector2 = penetration(u2.first,u1.first);
		Vec3 forceActingOn1 = -penetrationVector1*u2.first->stiffness();
		Vec3 forceActingOn2 = -penetrationVector2*u1.first->stiffness();
		forceActingOn1 -= forceActingOn2;
		u1.second += forceActingOn1;
	}

	void addForceDueToCollision(PhUnit& u1, StaticPhyscalUnitPtr u2) {
		Vec3 position = u1.first->getPosition();
		Vec3 penetrationVector = u2->penetration(position.x_,position.y_,u1.first->radius());
		Vec3 forceActingOn1 = -penetrationVector*u2->stiffness();
		u1.second += forceActingOn1;
	}

	void updateUnit(PhUnit& unit, double timeStep) {			
		Vec3 force = unit.second + unit.first->force();		
		unit.first->setVelocity(unit.first->getVelocity() + force / unit.first->mass() *timeStep);
		unit.first->setPosition(unit.first->getPosition() + unit.first->getVelocity() * timeStep);
		
		// Resets the force i.e. the current force is set to zero;
		unit.first->resetForce();
		unit.second = Vec3();
	}
	
	PhUnitList units_;
	StaticPhUnitList staticUnits_;
};

inline bool operator==(const PhysicalEngine::PhUnit& u1, const PhysicalEngine::PhUnit& u2) {
	return u1.first == u2.first;
}

}

#endif // PHYSICALENGINE_H
