#include "caranimation.h"
#include "typedefs.h"
#include "car.h"
#include "gamesprite.h"

#include <SDL_opengl.h>

namespace zombie {

	CarAnimation::CarAnimation(Car* car) : car_(car) {
		carSprite_ = carSprite;
	}

	CarAnimation::~CarAnimation() {
	}

	// private
	void CarAnimation::draw(float time, float timeStep, float accumulator) {
		State state = car_->getState();

		glPushMatrix();
		glColor3d(1,1,1);
		glTranslated(state.position_.x, state.position_.y, 0);
		glRotated(state.angle_ * 180 / PI,0,0,1);
		glRotated(270,0,0,1);
		glScaled(car_->getWidth(), car_->getLength(), 1);
		carSprite_.draw();
		glPopMatrix();
	}

} // Namespace zombie.
