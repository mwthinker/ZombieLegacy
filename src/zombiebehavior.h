#ifndef ZOMBIEBEHAVIOR_H
#define ZOMBIEBEHAVIOR_H

#include "input.h"
#include "unit.h"
#include "player.h"

#include <list>

namespace zombie {

	class Unit;

	class ZombieBehavior : public Player {
	public:
		ZombieBehavior(Unit* unit);
		~ZombieBehavior();

		void updatePhysics(float time, float deltaTime) override;

		MovingObject* getMovingObject() const override;

	private:
		MovingObject* findUninfectedTarget(Position position, const std::list<MovingObject*>& units) const;

		double findNewTargetTime_;

		double timeToUpdateAngleDirection_;
		double targetAngle_;
		bool forward_;
		Unit* unit_;
	};

} // Namespace zombie.

#endif // ZOMBIEBEHAVIOR_H
