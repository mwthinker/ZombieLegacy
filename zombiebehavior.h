#ifndef ZOMBIEBEHAVIOR_H
#define ZOMBIEBEHAVIOR_H

#include "aibehavior.h"
#include "typedefs.h"
#include "auxiliary.h"
#include "unit.h"

#include <mw/signal.h>
#include <list>

namespace zombie {

	class ZombieBehavior : public AiBehavior {
	public:
		ZombieBehavior();
		~ZombieBehavior();

		Input calculateInput(Unit* unit, double time) override;

		void unitEventHandler(Unit::UnitEvent unitEvent);
	private:
		MovingObject* findUninfectedTarget(Position position, const std::list<MovingObject*>& units) const;

		double findNewTargetTime_;
		MovingObject* target_;
		mw::signals::Connection connectionToTaget_;

		double timeToUpdateAngleDirection_;
		double targetAngle_;
		bool forward_;
	};

} // Namespace zombie.

#endif // ZOMBIEBEHAVIOR_H

