#include "physicalengine.h"

#include <mw/mathvector.h>

#include <list>
#include <utility>
#include <algorithm>

#include "unit.h"
#include "physicalunit.h"

namespace zombie {

	PhysicalEngine::PhysicalEngine(double x, double y, double width, double height) : staticUnits_(x,y,width,height,4) {
	}

	PhysicalEngine::~PhysicalEngine() {
	}

	// Adds a unit that is effected by the physics.
	void PhysicalEngine::add(const PhysicalUnitPtr& unit) {
		//units_.add(PhUnit(unit,Vec3()),unit->getPosition().x_,unit->getPosition().y_,unit->radius());
		units_.push_back(PhUnit(unit,Vec3()));
	}

	// Adds a static unit that only effect non static units.
	void PhysicalEngine::add(const StaticPhyscalUnitPtr& unit) {
		staticUnits_.add(unit,unit->getPosition().x_,unit->getPosition().y_,unit->getRadius());
	}

	// Removes the corresspondig unit.
	void PhysicalEngine::remove(const PhysicalUnitPtr& unit) {		
		units_.remove(PhUnit(unit,Vec3()));
	}

	// Removes the corresspondig static unit.
	void PhysicalEngine::remove(StaticPhyscalUnitPtr unit) {
		staticUnits_.remove(unit,unit->getPosition().y_,unit->getRadius());
	}

	// Simulates the physics for all units. The physics is simulated one time step.
	void PhysicalEngine::update(double timeStep) {
		// Calculates all contact interaction between units and buildings.
		for (auto it1 = units_.begin(); it1 != units_.end(); ++it1) {			
			PhUnit& u1 = *it1;

			// Interaction with other units.
			auto it2 = it1; 
			for (++it2; it2 != units_.end(); ++it2) {
				PhUnit& u2 = *it2;
				// Is unit1 and unit2 inside each other?
				if (inside(u1.first,u2.first)) {
					addForceDueToCollision(u1,u2);
				}
			}				

			// Interaction with static objects.
			auto staticUnits = staticUnits_.getObjectsAt(u1.first->getPosition().x_,u1.first->getPosition().y_,u1.first->radius());
			for (StaticPhyscalUnitPtr& staticUnit : staticUnits) {
				Vec3 p = u1.first->getPosition();
				if (staticUnit->isInsideApproximate(p.x_,p.y_,u1.first->radius())) {
					addForceDueToCollision(u1,staticUnit);
				}
			}
		}

		// Add friction and viscosity to all units.
		for (PhUnit& phUnit : units_) {
			addViscosity(phUnit);
			addFriction(phUnit);
			updateUnit(phUnit,timeStep);
		}

		// Remove all dead units.
		units_.remove_if([] (const PhUnit& phUnit) {
			return phUnit.first->toRemove();
		});		
	}

	void PhysicalEngine::addViscosity(PhUnit& u) {
		double speedSquared = u.first->getVelocity()*u.first->getVelocity();
		if (speedSquared > 0.00001*0.00001) { // In order to avoid dividing with zero when normallizing the velocity vector.			
			u.second += -10*speedSquared*u.first->getVelocity().normalize();
		}
	}

	void PhysicalEngine::addFriction(PhUnit& u) {
		double c = 0.1;
		Vec3 velocity = u.first->getVelocity();
		if (velocity.magnitude() > 0.0001) {
			Vec3 force = -c * u.first->mass()*velocity.normalize();
			u.second += force;
		}
	}

	Vec3 PhysicalEngine::penetration(const PhysicalUnitPtr& u1, const PhysicalUnitPtr& u2) {
		Vec3 diff = (u2->getPosition() - u1->getPosition());
		return diff+diff.normalize()*(u2->radius() - u1->radius());
	}

	// Add force due to collision to both units.
	void PhysicalEngine::addForceDueToCollision(PhUnit& u1, PhUnit& u2) {
		Vec3 penetrationVector1 = penetration(u1.first,u2.first);
		Vec3 penetrationVector2 = penetration(u2.first,u1.first);
		Vec3 forceActingOn1 = -penetrationVector1*u2.first->stiffness();
		Vec3 forceActingOn2 = -penetrationVector2*u1.first->stiffness();
		forceActingOn1 -= forceActingOn2;
		u1.second += forceActingOn1;
		u2.second -= forceActingOn1;
	}

	void PhysicalEngine::addForceDueToCollision(PhUnit& u1, StaticPhyscalUnitPtr u2) {
		Vec3 position = u1.first->getPosition();
		Vec3 penetrationVector = u2->penetration(position.x_,position.y_,u1.first->radius());
		Vec3 forceActingOn1 = -penetrationVector*u2->stiffness();
		u1.second += forceActingOn1;
	}

	void PhysicalEngine::updateUnit(PhUnit& unit, double timeStep) {			
		Vec3 force = unit.second + unit.first->force();		
		unit.first->setVelocity(unit.first->getVelocity() + force / unit.first->mass() *timeStep);
		unit.first->setPosition(unit.first->getPosition() + unit.first->getVelocity() * timeStep);

		// Resets the force i.e. the current force is set to zero;
		unit.first->resetForce();
		unit.second = Vec3();
	}
}
