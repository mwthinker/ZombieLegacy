#ifndef UNITANIMAITON_H
#define UNITANIMAITON_H

#include "animation.h"
#include "unit.h"

namespace zombie {

	class UnitAnimation {
	public:
		UnitAnimation(Unit* unit, const Animation& animation);
		~UnitAnimation();
	
		void draw(float time, float timeStep, float accumulator);
		void eventHandler(Unit::UnitEvent unitEvent);

	private:
		Unit* unit_;
		Animation animation_;
	};

} // Namespace zombie.

#endif //UNITANIMAITON_H
