#ifndef OBJECT_H
#define OBJECT_H

#include "box2ddef.h"

#include <cassert>

namespace zombie {

	// Represent a object in the box2d world inside "zombie world".
	class Object {	
	public:
		// Must call destroyBody() before destructor is called.
		inline virtual ~Object() {
		}

		// Is called when a collision occurs.
		inline virtual void collision(float impulse) {
		}

		// Return the body for the object. Use with care!
		// Return a nullpntr if the object not belongs to the world.
		virtual b2Body* getBody() const = 0;

		inline void setActive(bool active) {
			assert(getBody() != nullptr);
			getBody()->SetActive(active);
		}
		
		inline bool isActive() const {
			assert(getBody() != nullptr);
			return getBody()->IsActive();
		}

		inline void setAwake(bool awake) {
			assert(getBody() != nullptr);
			getBody()->SetAwake(awake);
		}

		inline bool isAwake() const {
			assert(getBody() != nullptr);
			return getBody()->IsAwake();
		}

	private:
		// Functions should only be called by the class ZombieEngine.
		friend class ZombieEngine;

		// Create the body. I.e. the body is now a part of the box2d world.
		virtual void createBody(b2World* world) = 0;

		// Destroys the body. I.e. the body in no more a part of the box2d world.
		virtual void destroyBody() = 0;
	};

} // Namespace zombie.

#endif // OBJECT_H
