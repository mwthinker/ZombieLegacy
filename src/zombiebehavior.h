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

		Input calculateInput(const Unit* unit, double time) override;

	private:
		MovingObject* findUninfectedTarget(Position position, const std::list<MovingObject*>& units) const;

		double findNewTargetTime_;

		double timeToUpdateAngleDirection_;
		double targetAngle_;
		bool forward_;
	};

} // Namespace zombie.

#endif // ZOMBIEBEHAVIOR_H
