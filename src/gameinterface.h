#ifndef GAMEINTERFACE_H
#define GAMEINTERFACE_H

#include "box2ddef.h"

namespace zombie {

	class Unit;
	class Car;
	class Bullet;
	class Building;

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

		virtual void collision(float impulse, Unit& unit) = 0;

		virtual void collision(float impulse, Car& car) = 0;

		virtual void collision(float impulse, Building& building) = 0;

		virtual void shotMissed(Position startPosition, Position hitPosition) = 0;

		virtual void shotHit(Position startPosition, Position hitPosition, Unit& unit) = 0;

	protected:
		~GameInterface() {
		}
	};

} // Namespace zombie.

#endif // GAMEINTERFACE_H
