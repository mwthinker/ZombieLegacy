#ifndef SHOCKWAVE_H
#define SHOCKWAVE_H

#include "box2ddef.h"
#include "auxiliary.h"

#include <mw/opengl.h>
#include <mw/color.h>
#include <mw/sprite.h>

#include <array>
#include <vector>

namespace zombie {

	class ShockWave {
	public:
		ShockWave(const mw::Sprite& sprite, float radiusSpeed, float expansionTime) {
			sprite_ = sprite;
			time_ = 0;
			expansionTime_ = expansionTime;
			radiusSpeed_ = radiusSpeed;
			color_ = mw::Color(1, 1, 1, 1);
		}

		void draw(float deltaTime) {
			time_ += deltaTime;
			if (time_ < expansionTime_) {
				color_.alpha_ = 1 - time_ / expansionTime_;
				color_.glColor4f();
				glPushMatrix();
				glScale2f(radiusSpeed_ * time_);
				sprite_.draw();
				glPopMatrix();
			}
		}

		bool isFinnish() const {
			return time_ > expansionTime_;
		}

		void restart() {
			time_ = 0;
		}

		void setColor(float red, float green, float blue) {
			color_.red_ = red;
			color_.green_ = green;
			color_.blue_ = blue;
		}

	private:
		mw::Sprite sprite_;
		float time_, expansionTime_;
		float radiusSpeed_;
		mw::Color color_;
	};

} // Namespace zombie.

#endif // SHOCKWAVE_H
