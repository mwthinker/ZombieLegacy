#include "caranimation.h"
#include "car.h"

#include "gamesprite.h"

#include <SDL_opengl.h>

namespace zombie {	

	CarAnimation::CarAnimation(Car* carParam) {
		inMemory_ = carParam->getInMemory();
		car_ = carParam;
		s_ = car;
	}

	CarAnimation::~CarAnimation() {
	}

	bool CarAnimation::isRunning() const {
		return inMemory_.isValid();
	}

	// private
	void CarAnimation::drawSecond(double time) {
		if (inMemory_.isValid()) {
			lastTime_ = time;

			glPushMatrix();
			State state = car_->state();

			glColor3d(1,1,1);
			glTranslated(state.position_.x,state.position_.y,0);
			glRotated(state.angle_*180/3.14,0,0,1);
			glRotated(270,0,0,1);
			double width = 1;
			double length = 2;
			glScaled(width,length,1);

			glScaled(2,2,1);
			/*
			glBegin(GL_QUADS);
			glColor3d(1,0,0);
			glVertex2d(-length*0.5,-width*0.5);
			glVertex2d(length*0.5,-width*0.5);
			glVertex2d(length*0.5,width*0.5);
			glVertex2d(-length*0.5,width*0.5);

			glColor3d(0,1,0);
			glVertex2d(length*0.4,-width*0.5);
			glVertex2d(length*0.5,-width*0.5);
			glVertex2d(length*0.5,width*0.5);
			glVertex2d(length*0.4,width*0.5);
			glEnd();
			*/
			s_.draw();		

			glPopMatrix();
		}
	}

} // Namespace zombie.
