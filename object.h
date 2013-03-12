#ifndef OBJECT_H
#define OBJECT_H

#include <Box2D/Dynamics/b2Body.h>

namespace zombie {

	// Represent a object inside the "zombie world".
	class Object {
	public:
		friend class ZombieGame;

		Object() {
		}		

		virtual ~Object() {
		}

		// Returns true if the position (x,y) is inside the object.
		//virtual bool isInside(double x, double y) const = 0;

		static b2World* getWorld() {
			return world_;
		}
	private:
		static void setWorld(b2World* world) {
			world_ = world;
		}

		static b2World* world_;
	};

} // namespace zombie

#endif // OBJECT_H