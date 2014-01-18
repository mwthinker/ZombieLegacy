#ifndef OBJECT_H
#define OBJECT_H

#include "box2ddef.h"

namespace zombie {

	class Player;

	// Represent a object inside the "zombie world".
	class Object {
	public:
		friend class Player;

		// Adds the object to the world. The world must point to an object.
		inline Object(b2World* world) : player_(nullptr), world_(world) {
		}

		virtual ~Object() {
		}

		// Return the body for the object. Use with care!
		virtual b2Body* getBody() const = 0;

		// Return the world for which the objects body is part of. Use with care!
		inline b2World* getWorld() {
			return world_;
		}

		// Return the player for which controls the object. May return null,
		// i.e. no one is controlling this object.
		Player* getPlayer() const {
			return player_;
		}

		// Draws the object in world coordinates.
		virtual void draw(float deltaTime) = 0;

	private:
		Player* player_;
		b2World* world_;
	};

} // Namespace zombie.

#endif // OBJECT_H
