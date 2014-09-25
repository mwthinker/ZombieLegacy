#ifndef OBJECT_H
#define OBJECT_H

#include "box2ddef.h"

#include <gui/windowmatrix.h>

#include <cassert>

namespace zombie {

	// Represent a physical object inside the "zombie world".
	class Object {
	public:
		// Must call destroyBody() before destructor is called.
		virtual ~Object() {
		}

		// Return the body for the object. Use with care!
		// Return a nullpntr if the object not belongs to the world.
		virtual b2Body* getBody() const = 0;

		// Destroys the body. I.e. the body in no more a part of the box2d world.
		virtual void destroyBody() = 0;

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
		virtual void draw(float accumulator, float timeStep, gui::WindowMatrixPtr wPtr) = 0;
	};

} // Namespace zombie.

#endif // OBJECT_H
