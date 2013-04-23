#ifndef SURVIVORBEHAVIOR_H
#define SURVIVORBEHAVIOR_H

#include "aibehavior.h"
#include "order.h"
#include "typedefs.h"
#include "auxiliary.h"

#include <vector>

namespace zombie {

	enum Activity { PANIC, FLEE, FOLLOW, DEFEND, ATTACK };
	enum Subroutines { TURN_TO_ANGLE, RELOAD };

	class SurvivorBehavior : public AiBehavior {
	public:
		SurvivorBehavior();
		~SurvivorBehavior();

		Input calculateInput(const Unit* unit, double time) override;
		Input activityToInput(Activity a, const Unit* unit, double time);
	private:
		Unit* findUninfectedTarget(Position position, const std::vector<Unit*>& units) const;

		double findNewTargetTime_;
		Unit* target_;

		double timeToUpdateAngleDirection_;
		double targetAngle_;
		bool forward_, backward_;
		std::vector<Subroutines> s_; 
	};

} // Namespace zombie.

#endif // ZOMBIEBEHAVIOR_H

