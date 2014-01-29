#ifndef PLAYER_H
#define PLAYER_H

#include "input.h"

namespace zombie {

	class MovingObject;
	class Driver;
	class Car;
	
	class Player {
	public:
		Player(MovingObject* mOb);
		virtual ~Player() {
		}

		Player(const Player&) = delete;
		Player& operator=(const Player&) = delete;

		virtual void updatePhysics(float time, float deltaTime) = 0;

		virtual MovingObject* getMovingObject() const = 0;

		virtual Driver* createDriver(Car* car) const {
			return nullptr;
		}
	};

} // Namespace zombie.

#endif // PLAYER_H
