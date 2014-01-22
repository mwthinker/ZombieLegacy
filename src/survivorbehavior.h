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
		SurvivorBehavior();
		~SurvivorBehavior();
		
		void updatePhysics(float time, float deltaTime) override;

	private:
		Unit* findUninfectedTarget(Position position, const std::vector<Unit*>& units) const;

		double findNewTargetTime_;
		Unit* target_;

		double timeToUpdateAngleDirection_;
		double targetAngle_;
		bool forward_, backward_;
		Input input_;
	};

} // Namespace zombie.

#endif // ZOMBIEBEHAVIOR_H
