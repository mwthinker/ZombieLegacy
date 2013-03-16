#ifndef MVOINGOBJECT_H
#define MVOINGOBJECT_H

#include "object.h"
#include "input.h"

namespace zombie {

	// Represent a moving object inside the "zombie world".
	class MovingObject : public Object {
	public:
		MovingObject() {
		}		

		virtual ~MovingObject() {
		}

		virtual void updatePhysics(float time, float timeStep, Input input) = 0;

		virtual void kill() = 0;
	};

} // namespace zombie

#endif // OBJECT_H