#ifndef EMITTER_H
#define EMITTER_H

#include "box2ddef.h"
#include "auxiliary.h"

#include <mw/opengl.h>
#include <mw/color.h>
#include <mw/sprite.h>

#include <array>

namespace zombie {

	template <int NBR_TRAILS>
	class Emitter {
	public:
		Emitter(const mw::Sprite& sprite, float minRadiusSpeed, float maxRadiusSpeed, float expansionTime) {
			minRadiusSpeed_ = minRadiusSpeed;
			maxRadiusSpeed_ = maxRadiusSpeed;
			sprite_ = sprite;
			expansionTime_ = 0.2f;
			time_ = 0;
			restart();
		}

		void draw(float deltaTime) {
			time_ += deltaTime;
			if (time_ < expansionTime_) {
				glColor4f(243 / 256.f, 255 / 256.f, 143 / 256.f, 0.9f);
				for (Trail& trail : trails_) {
					glPushMatrix();
					trail.update(deltaTime);
					glTranslate2f(trail.position_);
					glRotatef(trail.angle_ * 180 / PI);
					glScale2f(0.1f);
					glScalef(sprite_.getWidth(), sprite_.getHeight(), 1);
					sprite_.draw();
					glPopMatrix();
				}
			}
		}

		void restart() {
			time_ = 0.0;
			for (Trail& trail : trails_) {
				trail = Trail(minRadiusSpeed_, maxRadiusSpeed_);
			}
		}

	private:
		class Trail {
		public:
			inline Trail() {
			}

			inline Trail(float minRadiusSpeed, float maxRadiusSpeed) {
				position_ = ZERO;
				angle_ = random(0, 2 * PI);
				velocity_ = random(minRadiusSpeed, maxRadiusSpeed) * Velocity(std::cos(angle_), std::sin(angle_));
			}

			inline void update(float deltaTime) {
				// Update the position.
				position_ += deltaTime * velocity_;
			}

			Position position_;
			Velocity velocity_;
			float angle_;
		};

		float minRadiusSpeed_, maxRadiusSpeed_;

		std::array<Trail, NBR_TRAILS> trails_;
		mw::Sprite sprite_;
		float time_, expansionTime_;
	};

} // Namespace zombie.

#endif // EMITTER_H
