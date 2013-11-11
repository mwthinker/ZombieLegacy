#ifndef CARANIMATION_H
#define CARANIMATION_H

#include "graphicentity.h"

#include <mw/sprite.h>

namespace zombie {

	class Car;

	class CarAnimation : public GraphicEntity {
	public:
		CarAnimation(Car* car);
		~CarAnimation();

		void draw(float time, float timeStep, float accumulator) override;
	
	private:
		Car* car_;
		mw::Sprite carSprite_;
	};

} // Namespace zombie.

#endif // CARANIMATION_H
