#ifndef UNITANIMAITON_H
#define UNITANIMAITON_H

#include "task.h"
#include "animation.h"
#include "unit.h"

namespace zombie {

	class UnitAnimation : public Task {
	public:
		UnitAnimation(State state, float radius, const Animation& animation);
		
		// Updates the data describing the unit.
		void updateData(Unit* unit, Unit::UnitEvent unitEvent);

		// Draws the unit.
		bool update(float time) override;

	private:
		void eventHandler(Unit::UnitEvent unitEvent);

		State state_;
		float radius_;

		Animation animation_;
		bool remove_;
	};

} // Namespace zombie.

#endif //UNITANIMAITON_H
