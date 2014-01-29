#ifndef OBJECT_H
#define OBJECT_H

#include "box2ddef.h"

#include <cassert>

namespace zombie {

	class Car;
	class Unit;
	class Building;

	// Represent a object inside the "zombie world".
	class Object {
	public:
		// Should remove the object from the world!
		virtual ~Object() {
		}

		// Creates a body!
		virtual void createBody(b2World* world) = 0;

		// Deallocates the body.
		virtual void destroyBody(b2World* world) = 0;

		virtual bool isDestroyed() const = 0;

		// Return the body for the object. Use with care!
		virtual b2Body* getBody() const = 0;

		inline virtual void update(float time, float timeStep) {
		}
		
		virtual bool toBeRemoved() const {
			return false;
		}

		// Draw the object in world coordinates.
		virtual void draw(float deltaTime) = 0;

		// Implements a double dispatch. Must be overridden and
		// inside the function call ob->collisionWith(this).
		virtual void collisionWith(Object* ob, float impulse) = 0;
		
		virtual void collisionWith(Unit*, float impulse);

		virtual void collisionWith(Car*, float impulse);

		virtual void collisionWith(Building*, float impulse);
	};

} // Namespace zombie.

#endif // OBJECT_H
