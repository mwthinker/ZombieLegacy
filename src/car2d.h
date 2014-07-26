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
		void draw(float accumulator, float timeStep) override {
			// Draw body.
			const float alpha = accumulator / timeStep;

			State state = getState();
			state.position_ = alpha * state.position_ + (1.f - alpha) * previousState().position_;

			glPushMatrix();
			glTranslate2f(state.position_);
			glRotated(state.angle_ * 180 / PI, 0, 0, 1);
			glRotated(270, 0, 0, 1);
			glScaled(getWidth(), getWidth(), 1);
			animation_.draw(timeStep);
			glPopMatrix();
		}

	private:
		Animation animation_;
		State previousState_;
	};
	
} // Namespace zombie.

#endif // CAR2D_H
