#ifndef MVOINGOBJECT_H
#define MVOINGOBJECT_H

#include "object.h"
#include "input.h"

#include "weapon.h"
#include "typedefs.h"

#include <mw/signal.h>

#include <list>

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

		const std::list<MovingObject*>& getVisibleObjects() const {
			return visibleObjects_;
		}

		void addVisibleObject(MovingObject* object) {
			visibleObjects_.push_back(object);
		}

		void removeVisibleObject(MovingObject* object) {
			visibleObjects_.erase(std::find(visibleObjects_.begin(), visibleObjects_.end(), object));
		}

		mw::signals::Connection addExistHandler() {
			return existSignal_.connect([]() {});
		}

	private:
		std::list<MovingObject*> visibleObjects_;
		mw::Signal<void> existSignal_;
	};

} // namespace zombie

#endif // OBJECT_H