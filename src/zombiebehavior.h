#ifndef ZOMBIEBEHAVIOR_H
#define ZOMBIEBEHAVIOR_H

#include "aibehavior.h"
#include "typedefs.h"
#include "input.h"
#include "unit.h"

#include <list>

namespace zombie {

	class ZombieBehavior : public AiBehavior {
	public:
		ZombieBehavior();
		~ZombieBehavior();

		void calculateInput(const Unit* unit, double time) override;

		inline Input getInput() const override {
			return input_;
		}

	private:
		MovingObject* findUninfectedTarget(Position position, const std::list<MovingObject*>& units) const;

		double findNewTargetTime_;

		double timeToUpdateAngleDirection_;
		double targetAngle_;
		bool forward_;

		Input input_;
	};

} // Namespace zombie.

#endif // ZOMBIEBEHAVIOR_H
