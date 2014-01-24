#ifndef CAR2D_H
#define CAR2D_H

#include "car.h"
#include "auxiliary.h"

#include <mw/sprite.h>

#include <SDL_opengl.h>

namespace zombie {

	class Car2D : public Car {
	public:
		Car2D(b2World* world, const State& state, float mass, float life, float width, float length, const mw::Sprite& sprite) : 
			Car(world, state, mass, life, width, length), animation_(sprite) {
		}

		// Draws the car.
		void draw(float time) override {
			// Draw body.
			State state = getState();

			glPushMatrix();
			glTranslate2f(state.position_);
			glRotated(state.angle_ * 180 / PI, 0, 0, 1);
			glScaled(getLength(), getWidth(), 1);
			glRotated(270, 0, 0, 1);
			animation_.draw();
			glPopMatrix();
		}

	private:
		mw::Sprite animation_;
	};
	
} // Namespace zombie.

#endif // CAR2D_H
