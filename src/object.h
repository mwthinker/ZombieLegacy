#ifndef OBJECT_H
#define OBJECT_H

#include "box2ddef.h"
#include "gameshader.h"

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

		// Is called when a collision occurs.
		virtual void collision(float impulse) {
		}
	};	

} // Namespace zombie.

#endif // OBJECT_H
