#include "caranimation.h"

#include "typedefs.h"
#include "car.h"
#include "gamesprite.h"

#include <SDL_opengl.h>

namespace zombie {	

	CarAnimation::CarAnimation(Car* car) : idCarObject_(car->getId()) {
		carSprite_ = carSprite;
	}

	CarAnimation::~CarAnimation() {
	}

	bool CarAnimation::isRunning() const {
		return Object::getObject(idCarObject_) != nullptr;
	}

	// private
	void CarAnimation::draw(double time) {
		const Object* ob = Object::getObject(idCarObject_);
		if (ob != nullptr) {
			const Car* car = static_cast<const Car*>(ob);
		
			lastTime_ = time;

			glPushMatrix();
			State state = car->state();

			glColor3d(1,1,1);
			glTranslated(state.position_.x,state.position_.y,0);
			glRotated(state.angle_*180/3.14,0,0,1);
			glRotated(270,0,0,1);
			double width = 1;
			double length = 2;
			glScaled(width,length,1);

			glScaled(2,2,1);

			carSprite_.draw();

			glPopMatrix();
		}
	}

} // Namespace zombie.
