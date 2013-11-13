#ifndef PLAYER_H
#define PLAYER_H

#include "input.h"
#include "movingobject.h"

namespace zombie {

	class Player {
	public:
		virtual ~Player() {
		}

		virtual void updatePhysics(float time, float deltaTime) = 0;

		virtual MovingObject* getMovingObject() const = 0;
	};

} // Namespace zombie.

#endif // PLAYER_H
