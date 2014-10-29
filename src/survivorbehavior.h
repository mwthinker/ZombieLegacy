#ifndef SURVIVORBEHAVIOR_H
#define SURVIVORBEHAVIOR_H

#include "input.h"
#include "box2ddef.h"
#include "unit.h"
#include "player.h"

#include <vector>

namespace zombie {

	class SurvivorBehavior : public Player {
	public:
		SurvivorBehavior(Unit* unit);
		~SurvivorBehavior();
		
		void updateInput(float time, float deltaTime) override;

		MovingObject* getMovingObject() const;

	private:
		Unit* findUninfectedTarget(Position position, const std::vector<Unit*>& units) const;

		double findNewTargetTime_;
		Unit* target_;
		Unit* unit_;
	};

} // Namespace zombie.

#endif // ZOMBIEBEHAVIOR_H
