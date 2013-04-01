#ifndef MVOINGOBJECT_H
#define MVOINGOBJECT_H

#include "object.h"
#include "input.h"

#include "weapon.h"
#include "typedefs.h"

#include <mw/signal.h>

#include <list>

namespace zombie {	

	// Represent a moving object inside the "zombie world".
	class MovingObject : public Object {
	public:
		friend class ZombieGame;

		virtual ~MovingObject() {
		}
		
		// Update all internal states based on the input.
		virtual void updatePhysics(float time, float timeStep, Input input) = 0;		

		// Returns the current weapon.
		virtual Weapon getWeapon() const = 0;

		// Return the objects forward direction in radians.
		virtual float getDirection() const = 0;

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
			return visibleObjects_;
		}		

		// Returns a connected connection that will be disconnected when this 
		// object removes from memory.
		mw::signals::Connection addExistHandler() {
			return existSignal_.connect([]() {});
		}

	private:
		void addVisibleObject(MovingObject* object) {
			visibleObjects_.push_back(object);
		}

		void removeVisibleObject(MovingObject* object) {
			visibleObjects_.erase(std::find(visibleObjects_.begin(), visibleObjects_.end(), object));
		}
		
		std::list<MovingObject*> visibleObjects_;
		mw::Signal<void> existSignal_;
	};

} // namespace zombie

#endif // OBJECT_H