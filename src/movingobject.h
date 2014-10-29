#ifndef MOVINGOBJECT_H
#define MOVINGOBJECT_H

#include "object.h"
#include "input.h"
#include "weapon.h"
#include "state.h"

#include <mw/signal.h>

#include <list>
#include <vector>
#include <algorithm>

namespace zombie {

	// Represent a moving object inside the "zombie world".
	class MovingObject : public Object {
	public:
		inline void setInput(Input input) {
			input_ = input;
		}

		inline Input getInput() const {
			return input_;
		}

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

		// Remove dead objects.
		virtual bool toBeRemoved() const final {
			return isDead();
		}

		// Returns a reference to the current list of viewable moving objects.
		// The objects are only guaranteed to exist in the current game time.
		const std::list<MovingObject*>& getVisibleObjects() const {
			return objectsSeen_;
		}

		// Should only be called by the game engine.
		void addSeenObject(MovingObject* object) {
			objectsSeen_.push_back(object);
		}

		// Should only be called by the game engine.
		void removeSeenObject(MovingObject* object) {
			objectsSeen_.remove(object);
		}

		void setActive(bool active) {
			getBody()->SetActive(active);
		}

		bool isActive() const {
			return getBody()->IsActive();
		}

	private:
		std::list<MovingObject*> objectsSeen_;
		Input input_;
	};

} // Namespace zombie.

#endif // MOVINGOBJECT_H
