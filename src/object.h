#ifndef OBJECT_H
#define OBJECT_H

#include "box2ddef.h"

#include <cassert>

namespace zombie {

	// Represent a object inside the "zombie world".
	class Object {
	public:
		// Should remove the object from the world!
		virtual ~Object() {
		}

		// Deallocates the body.
		virtual void destroyBody(b2World* world) = 0;

		// Return the body for the object. Use with care!
		virtual b2Body* getBody() const = 0;

		// Update the object. The parameter is the current "game" time and the time step is 
		// the length of the time to be simultaded in the updated call.
		inline virtual void update(float time, float timeStep) {
		}
		
		// Return whether when the game engine should remove the object or if it should continue to exist.
		virtual bool toBeRemoved() const {
			return false;
		}

		// Draw the object in world coordinates.
		virtual void draw(float deltaTime) = 0;

	};

} // Namespace zombie.

#endif // OBJECT_H
