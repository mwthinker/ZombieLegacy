#ifndef PLAYER_H
#define PLAYER_H

#include "object.h"
#include "input.h"
#include "movingobject.h"

namespace zombie {
	
	class Player {
	public:
		Player(Object* object) {
			object->player_ = this;
		}

		virtual ~Player() {
		}

		virtual void updatePhysics(float time, float deltaTime) = 0;

		virtual MovingObject* getMovingObject() const = 0;
	};

} // Namespace zombie.

#endif // PLAYER_H
