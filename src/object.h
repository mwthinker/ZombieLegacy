#ifndef OBJECT_H
#define OBJECT_H

#include "box2ddef.h"

#include <cassert>

namespace zombie {

	// Represent a object inside the "zombie world".
	class Object {
	public:
		// Should add the object to the world. The world must point to an object.
		inline Object(b2World* world) : world_(world) {
			assert(world); // Should not be null!
		}

		// Should remove the object from the world!
		virtual ~Object() {
		}

		// Return the body for the object. Use with care!
		virtual b2Body* getBody() const = 0;

		// Return the world for which the objects body is part of. Use with care!
		inline b2World* getWorld() {
			return world_;
		}

		inline virtual void update(float time, float timeStep) {
		}

		// Draw the object in world coordinates.
		virtual void draw(float deltaTime) = 0;

	private:
		b2World* world_;
	};

} // Namespace zombie.

#endif // OBJECT_H
