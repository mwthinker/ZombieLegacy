#ifndef PHYSICALENGINE_H
#define PHYSICALENGINE_H

#include "quadtree.h"
#include "typedefs.h"

#include <mw/mathvector.h>

#include <list>
#include <utility>
#include <algorithm>

namespace zombie {

	class StaticPhyscalUnit {
	public:
		virtual ~StaticPhyscalUnit() {
		}

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

		virtual double getRadius() const = 0;
		virtual Position getPosition() const = 0;
	};	

	bool inside(const PhysicalUnitPtr& unit1, const PhysicalUnitPtr& unit2);

	class PhysicalEngine {
	public:
		PhysicalEngine(double x, double y, double width, double height);
		~PhysicalEngine();

		// Adds a unit that is effected by the physics.
		void add(const PhysicalUnitPtr& unit);

		// Adds a static unit that only effect non static units.
		void add(const StaticPhyscalUnitPtr& unit);

		// Removes the corresspondig unit.
		void remove(const PhysicalUnitPtr& unit);

		// Removes the corresspondig static unit.
		void remove(StaticPhyscalUnitPtr unit);

		// Simulates the physics for all units. The physics is simulated one time step.
		void update(double timeStep);
	
	private:
		typedef std::pair<PhysicalUnitPtr, Vec3> PhUnit;

		friend bool operator==(const PhUnit&, const PhUnit&);

		void addViscosity(PhUnit& u);

		void addFriction(PhUnit& u);

		Vec3 penetration(const PhysicalUnitPtr& u1, const PhysicalUnitPtr& u2);

		// Add force due to collision to both units.
		void addForceDueToCollision(PhUnit& u1, PhUnit& u2);

		void addForceDueToCollision(PhUnit& u1, StaticPhyscalUnitPtr u2);

		void updateUnit(PhUnit& unit, double timeStep);

		std::list<PhUnit> units_;
		Quadtree<StaticPhyscalUnitPtr> staticUnits_;
	};

	inline bool operator==(const PhysicalEngine::PhUnit& u1, const PhysicalEngine::PhUnit& u2) {
		return u1.first == u2.first;
	}

}

#endif // PHYSICALENGINE_H

