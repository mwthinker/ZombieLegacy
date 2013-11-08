#ifndef MOVINGUNITLISTERNER_H
#define MOVINGUNITLISTERNER_H

namespace zombie {

	class MovingUnit;
	class Bullet;

	class MovingUnitListener {
	public:
		virtual void shoot(MovingUnit* unit, const Bullet& bullet) = 0;

		virtual void reload(MovingUnit* unit, const Bullet& bullet) = 0;
	};

} // Namespace zombie.

#endif // MOVINGUNITLISTERNER_H
