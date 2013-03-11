#ifndef OBJECT_H
#define OBJECT_H

#include <Box2D/Dynamics/b2Body.h>

namespace zombie {

	// Represent a object inside the "zombie world".
	class Object {
	public:
		Object() {
		}

		virtual ~Object() {
		}

		// Returns true if the position (x,y) is inside the object.
		//virtual bool isInside(double x, double y) const = 0;
	};

} // namespace zombie

#endif // OBJECT_H