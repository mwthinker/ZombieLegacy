#ifndef GAMEINTERFACE_H
#define GAMEINTERFACE_H

#include "box2ddef.h"

namespace zombie {

	class Unit;
	class Car;
	class Building;
	struct MissileProperties;

	class GameInterface {
	public:
		// A unit just died.
		virtual void unitDied(Unit& unit) = 0;

		virtual void collision(float impulse, Unit& unit) = 0;

		virtual void collision(float impulse, Car& car) = 0;

		virtual void collision(float impulse, Building& building) = 0;

		virtual void shotMissed(Position startPosition, Position hitPosition) = 0;

		virtual void shotHit(Position startPosition, Position hitPosition, Unit& unit) = 0;

		virtual void explosion(Position position, float explosionRadius) = 0;

		virtual void shot(Unit& shooter, float speed, float explodeTime, float damage, float explosionRadius) {
		};

		virtual void removedFromWorld(Unit& unit) = 0;

	protected:
		~GameInterface() {
		}
	};

} // Namespace zombie.

#endif // GAMEINTERFACE_H
