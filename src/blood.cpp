#include "blood.h"
#include "auxiliary.h"

#include <SDL_opengl.h>

namespace zombie {

	Blood::Blood(Position position) {
		time_ = 0;
		position_ = position;
		//blood_ = bloodSprite;
		duration_ = 0.15f;
	}

	void Blood::draw(float deltaTime) {
		time_ += deltaTime;
		if (time_ < duration_) {
			glPushMatrix();
			glColor3d(0.4, 0.4, 0.4);
			glTranslate2f(position_);
			float scale = 1 + 3.5f * time_ / duration_;
			glScale2f(scale);
			//blood_.draw();
			glPopMatrix();
		} else {
			glPushMatrix();
			glColor3d(0.4, 0.4, 0.4);
			glTranslate2f(position_);
			glScaled(3, 3, 1);
			//blood_.draw();
			glPopMatrix();
		}
	}

	bool Blood::toBeRemoved() const {
		return time_ > 2 * duration_;
	}

} // Namespace zombie.
