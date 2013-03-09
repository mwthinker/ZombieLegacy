#ifndef ZOMBIEBEHAVIOR_H
#define ZOMBIEBEHAVIOR_H

#include "aibehavior.h"
#include "typedefs.h"
#include "auxiliary.h"

#include <vector>

namespace zombie {

	class ZombieBehavior : public AiBehavior {
	public:
		ZombieBehavior();
		~ZombieBehavior();

		Input calculateInput(const UnitPtr& unit, const std::vector<UnitPtr>& units, double time) override;

	private:
		UnitPtr findUninfectedTarget(Position position, const std::vector<UnitPtr>& units) const;

		double findNewTargetTime_;
		UnitPtr target_;

		double timeToUpdateAngleDirection_;
		double targetAngle_;
		bool forward_;
	};

} // Namespace zombie.

#endif // ZOMBIEBEHAVIOR_H

