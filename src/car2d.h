#ifndef CAR2D_H
#define CAR2D_H

#include "car.h"
#include "animation.h"
#include "gamedataentry.h"

#include <mw/opengl.h>

namespace zombie {

	class Car2D : public Car {
	public:
		Car2D(float mass, float life, float width, float length, const Animation& animation);

		// Draws the car.
		void draw(float accumulator, float timeStep, gui::WindowMatrixPtr wPtr) override;

	private:
		Animation animation_;
		State previousState_;
	};

	Car2D loadCar(GameDataEntry& entry);
	
} // Namespace zombie.

#endif // CAR2D_H
