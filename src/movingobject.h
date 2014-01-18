#ifndef MOVINGOBJECT_H
#define MOVINGOBJECT_H

#include "object.h"
#include "input.h"
#include "weapon.h"
#include "state.h"

#include <list>
#include <vector>
#include <algorithm>

namespace zombie {

	// Represent a moving object inside the "zombie world".
	class MovingObject : public Object {
	public:
		friend class ZombieEngine;

		MovingObject(b2World* world) : Object(world) {
		}

		virtual ~MovingObject() {
		}

		// Simulates the physics at time (time) one time step (timeStep) ahead.
		// Based on the input given.
		virtual void updatePhysics(float time, float timeStep, Input input) = 0;

		// Simulates a collision with the force defined in impulse.
		virtual void collision(float impulse) = 0;

		// Returns the current weapon.
		virtual Weapon getWeapon() const = 0;

		// Return the objects forward direction in radians.
		virtual float getDirection() const = 0;

		// Return the current state.
		virtual State getState() const = 0;

		// Returns true if the object is infected.
		virtual bool isInfected() const = 0;

		// Returns the centre position of the object.
		virtual Position getPosition() const = 0;

		// Returns the distance of the view radius. I.e. all objects
		// inside the circle centered in getPosition with radius view distance is
		// seen by this object.
		virtual float getViewDistance() const = 0;

		// Returns true if the position is inside the view area.
		virtual bool isInsideViewArea(Position position) const = 0;

		// Returns true if object is dead.
		virtual bool isDead() const = 0;

		// Returns a reference to the current list of viewable moving objects.
		// The objects are only guaranteed to exist in the current game time.
		const std::list<MovingObject*>& getVisibleObjects() const {
			return objectsSeen_;
		}		

	private:
		void addSeenObject(MovingObject* object) {
			objectsSeen_.push_back(object);
		}

		void removeSeenObject(MovingObject* object) {
			objectsSeen_.erase(std::find(objectsSeen_.begin(), objectsSeen_.end(), object));
		}

		std::list<MovingObject*> objectsSeen_;
	};

} // Namespace zombie.

#endif // MOVINGOBJECT_H
