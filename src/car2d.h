#ifndef CAR2D_H
#define CAR2D_H

#include "car.h"
#include "auxiliary.h"
#include "animation.h"

#include <SDL_opengl.h>

namespace zombie {

	class Car2D : public Car {
	public:
		Car2D(float mass, float life, float width, float length, const Animation& animation) :
			Car(mass, life, width, length), animation_(animation) {
		}

		// Draws the car.
		void draw(float time) override {
			// Draw body.
			State state = getState();

			glPushMatrix();
			glTranslate2f(state.position_);
			glRotated(state.angle_ * 180 / PI, 0, 0, 1);
			glRotated(270, 0, 0, 1);
			glScaled(getWidth(), getWidth(), 1);
			animation_.draw(time);
			glPopMatrix();
		}

	private:
		Animation animation_;
	};
	
} // Namespace zombie.

#endif // CAR2D_H
