#ifndef OBJECT_H
#define OBJECT_H

#include "box2ddef.h"

#include <cassert>

namespace zombie {

	// Represent a physical object inside the "zombie world".
	class Object {
	public:
		Object() : humanPosition_(ZERO) {
		}
		
		// Must call destroyBody() before destructor is called.
		virtual ~Object() {
		}

		// Return the body for the object. Use with care!
		virtual b2Body* getBody() const = 0;

		void destroyBody();

		// Update the object. The "time" parameter is the current game time and the "timeStep" is 
		// the length of the time to be simulated in the updated call.
		inline virtual void update(float time, float timeStep) {
		}
		
		// Return whether when the game engine should remove the object or if it should continue to exist.
		virtual bool toBeRemoved() const {
			return false;
		}

		// Is called when a collision occurs.
		virtual void collision(float impulse) {
		}

		// Draw the object in world coordinates.
		virtual void draw(float accumulator, float timeStep) = 0;

		inline Position humanPosition() const {
			return humanPosition_;
		}

		inline void setHumanPosition(const Position& position) {
			humanPosition_ = position;
		}

	private:
		Position humanPosition_;
	};

} // Namespace zombie.

#endif // OBJECT_H
