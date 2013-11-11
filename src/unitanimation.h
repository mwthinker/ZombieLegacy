#ifndef UNITANIMAITON_H
#define UNITANIMAITON_H

#include "GraphicEntity.h"
#include "animation.h"
#include "unit.h"

namespace zombie {

	class Unit;

	class UnitAnimation : public GraphicEntity {
	public:
		UnitAnimation(Unit* unit, const Animation& animation);
		~UnitAnimation();
	
		void draw(float time, float timeStep, float accumulator) override;
		void eventHandler(Unit::UnitEvent unitEvent);

	private:
		Unit* unit_;
		Animation animation_;
	};

} // Namespace zombie.

#endif //UNITANIMAITON_H
