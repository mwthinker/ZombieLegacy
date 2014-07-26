#ifndef GAMEINTERFACE_H
#define GAMEINTERFACE_H

#include "box2ddef.h"

namespace zombie {

	class Unit;
	class Car;
	class Bullet;

	class GameInterface {
	public:
		// Is called for each unit every game cycle.
		virtual void updateEachCycle(Unit& unit, Unit& human) = 0;

		// Is called for the human every game cycle.
		virtual void updateEachCycle(Unit& human) = 0;

		// The human just died.
		virtual void humanDied(Unit& unit) = 0;

		// A unit just died.
		virtual void unitDied(Unit& unit) = 0;

		virtual void collision(float impulse, Car& car, Unit& unit) = 0;

		virtual void collision(float impulse, Car& car1, Car& car2) = 0;

		virtual void collision(float impulse, Car& car, Building& building) = 0;

		virtual void shotMissed(const Bullet& bullet, Position end) = 0;

		virtual void shotHit(const Bullet& bullet, Unit& unit) = 0;

	protected:
		~GameInterface() {
		}
	};

} // Namespace zombie.

#endif // GAMEINTERFACE_H
