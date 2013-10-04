#include "blood.h"
#include "task.h"
#include "gamesound.h"
#include "gamesprite.h"

#include <SDL_opengl.h>

namespace zombie {

	BloodSplash::BloodSplash(double x, double y, double currentTime) {
		startTime_ = currentTime;
		x_ = x;
		y_ = y;
	}

	bool  BloodSplash::update(double time) {
		if (time < startTime_ + 0.1) {
			// Draw view sphere.
			glColor3d(1,0,0);
			drawCircle(x_,y_,(time-startTime_)*3.5,10,true);
			return true;
		} else {
			return false;
		}
	}

	Blood::Blood(double x, double y, double currentTime) {
		startTime_ = currentTime;
		x_ = x;
		y_ = y;
		blood_ = bloodSprite;
		startScaleX_ = 1;
		startScaleY_ = 1;
		endScaleX_ = 3.5;
		endScaleY_ = 3.5;
		duration_ = 0.15;
	}

	bool Blood::update(double time) {
		double dT = time - startTime_;
		if (dT < duration_) {
			// Draw view sphere.
			glPushMatrix();
			glColor3d(0.4,0.4,0.4);
			glTranslated(x_,y_,0);
			double s = startScaleX_ + (startScaleX_+endScaleX_)*(dT/duration_);
			glScaled(s,s,1);
			blood_.draw();
			glPopMatrix();
			return true;
		}
		return false;
	}

	BloodStain::BloodStain(double x, double y, double currentTime) {
		startTime_ = currentTime;
		x_ = x;
		y_ = y;
		blood_ = bloodSprite;
		scaleX_ = 3;
		scaleY_ = 3;
		delay_ = 0.15;	// A stain appears after the splash is finished.
		duration_ = 60;
		mw::Sound tmp = splat;
		tmp.play();
	}

	bool BloodStain::update(double time) {
		double dT = time - startTime_;
		if (dT > delay_ && dT < duration_) {
			// Draw view sphere
			glPushMatrix();
			glColor3d(0.4,0.4,0.4);			
			glTranslated(x_,y_,0);
			glScaled(scaleX_,scaleY_,1);	 	
			blood_.draw();
			glPopMatrix();
			return true;
		} else if (dT > delay_) {
			return false;
		}
		return true;
	}

} // Namespace zombie.
