#ifndef CAR2D_H
#define CAR2D_H

#include "car.h"
#include "auxiliary.h"
#include "animation.h"

#include <mw/opengl.h>

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

			glColor3d(1, 1, 1);
			glPushMatrix();
			glTranslate2f(state.position_);
			glRotatef(state.angle_ * 180 / PI);
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
