#include "caranimation.h"
#include "car.h"

#include <SDL_opengl.h>

namespace zombie {	

	CarAnimation::CarAnimation(Car* carParam) {
		car_ = carParam;
		s_ = car;
	}

	CarAnimation::~CarAnimation() {
	}

	void CarAnimation::drawSecond(double time) {
		lastTime_ = time;
		draw(0.0);
	}

	bool CarAnimation::isRunning() const {
		return true;
	}

	// private
	void CarAnimation::draw(double timestep) {
		glPushMatrix();
		State state = car_->state();

		glTranslated(state.position_.x,state.position_.y,0);
		glRotated(state.angle_*180/3.14,0,0,1);
		glRotated(270,0,0,1);
		double width = 1;
		double length = 2;

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

} // Namespace zombie.