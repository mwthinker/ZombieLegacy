#ifndef SURVIVORBEHAVIOR_H
#define SURVIVORBEHAVIOR_H

#include "input.h"
#include "aibehavior.h"
#include "typedefs.h"

#include <vector>

namespace zombie {

	class Unit;

	enum Activity { PANIC, FLEE, FOLLOW, DEFEND, ATTACK };
	enum Subroutines { TURN_TO_ANGLE, RELOAD };

	class SurvivorBehavior : public AiBehavior {
	public:
		SurvivorBehavior();
		~SurvivorBehavior();

		void calculateInput(const Unit* unit, double time) override;
		Input activityToInput(Activity a, const Unit* unit, double time);

		inline Input getInput() const override {
			return input_;
		}

	private:
		Unit* findUninfectedTarget(Position position, const std::vector<Unit*>& units) const;

		double findNewTargetTime_;
		Unit* target_;

		double timeToUpdateAngleDirection_;
		double targetAngle_;
		bool forward_, backward_;
		std::vector<Subroutines> s_; 
		Input input_;
	};

} // Namespace zombie.

#endif // ZOMBIEBEHAVIOR_H
