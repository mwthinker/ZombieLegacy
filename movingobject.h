#ifndef MVOINGOBJECT_H
#define MVOINGOBJECT_H

#include "object.h"
#include "input.h"

#include "weapon.h"
#include "typedefs.h"

namespace zombie {

	// Represent a moving object inside the "zombie world".
	class MovingObject : public Object {
	public:
		virtual ~MovingObject() {
		}
		
		virtual void updatePhysics(float time, float timeStep, Input input) = 0;		

		virtual Weapon getWeapon() const = 0;

		virtual float getDirection() const = 0;

		virtual bool isInfected() const = 0;

		virtual Position getPosition() const = 0;
		virtual float getViewDistance() const = 0;

		virtual bool isInsideViewArea(Position position) const = 0;

		virtual bool isDead() const = 0;
	};

} // namespace zombie

#endif // OBJECT_H