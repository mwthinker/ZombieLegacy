#ifndef ZOMBIEBEHAVIOR_H
#define ZOMBIEBEHAVIOR_H

#include "aibehavior.h"
#include "typedefs.h"
#include "auxiliary.h"
#include "unit.h"

#include <mw/signal.h>
#include <vector>

namespace zombie {

	class ZombieBehavior : public AiBehavior {
	public:
		ZombieBehavior();
		~ZombieBehavior();

		Input calculateInput(Unit* unit, const std::vector<Unit*>& units, double time) override;

		void unitEventHandler(Unit::UnitEvent unitEvent);
	private:
		Unit* findUninfectedTarget(Position position, const std::vector<Unit*>& units) const;

		double findNewTargetTime_;
		Unit* target_;
		mw::signals::Connection connectionToTaget_;

		double timeToUpdateAngleDirection_;
		double targetAngle_;
		bool forward_;
	};

} // Namespace zombie.

#endif // ZOMBIEBEHAVIOR_H

