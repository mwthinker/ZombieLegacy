#ifndef SURVIVORBEHAVIOR_H
#define SURVIVORBEHAVIOR_H

#include "input.h"
#include "player.h"
#include "box2ddef.h"

#include <vector>

namespace zombie {

	class Unit;

	class SurvivorBehavior : public Player {
	public:
		SurvivorBehavior(Unit* unit);
		~SurvivorBehavior();
		
		void updatePhysics(float time, float deltaTime) override;

		MovingObject* getMovingObject() const override;

	private:
		Unit* findUninfectedTarget(Position position, const std::vector<Unit*>& units) const;

		double findNewTargetTime_;
		Unit* target_;
		Unit* unit_;
	};

} // Namespace zombie.

#endif // ZOMBIEBEHAVIOR_H
