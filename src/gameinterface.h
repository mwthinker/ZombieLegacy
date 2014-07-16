#ifndef GAMEINTERFACE_H
#define GAMEINTERFACE_H

#include "unit.h"
#include "car.h"
#include "building.h"

namespace zombie {

	class GameInterface {
	protected:
		~GameInterface() {
		}

	public:

		// 
		virtual void updateSpawning(Unit& human) {
		}

		// The current position of the human.
		virtual void currentHuman(Unit& unit) {
		}

		// The human just died.
		virtual void humanDied(Unit& unit) {
		}

		// A unit just died.
		virtual void unitDied(Unit& unit) {
		}

		// Unit is removed if the function returns true.
		virtual bool isUnitOutside(float x, float y, bool infected) {
			return false;
		}

		virtual void collision(float impulse, Car& car, Unit& unit) {
		}

		virtual void collision(float impulse, Car& car1, Car& car2) {
		}

		virtual void collision(float impulse, Car& car, Building& building) {
		}

		virtual void shotMissed(const Bullet& bullet) {
		}

		virtual void shotHit(const Bullet& bullet, Unit& unit) {
		}
	};

} // Namespace zombie.

#endif // GAMEINTERFACE_H
