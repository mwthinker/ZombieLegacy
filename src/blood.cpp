#include "blood.h"
#include "gamesprite.h"
#include "auxiliary.h"

#include <SDL_opengl.h>

namespace zombie {

	Blood::Blood(Position position, float currentTime) {
		startTime_ = currentTime;
		position_ = position;
		blood_ = bloodSprite;
		duration_ = 0.15f;
	}

	bool Blood::update(float time) {
		float dT = time - startTime_;
		if (dT < duration_) {
			glPushMatrix();
			glColor3d(0.4,0.4,0.4);
			glTranslate2f(position_);
			float scale = 1 + 3.5f * dT / duration_;
			glScale2f(scale);
			blood_.draw();
			glPopMatrix();
			return true;
		} else if (dT < 2 * duration_) {
			glPushMatrix();
			glColor3d(0.4, 0.4, 0.4);
			glTranslate2f(position_);
			glScaled(3, 3, 1);
			blood_.draw();
			glPopMatrix();
			return true;
		}
		return false;
	}

} // Namespace zombie.
