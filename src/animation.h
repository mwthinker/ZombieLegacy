#ifndef ANIMATION_H
#define ANIMATION_H

#include <mw/sprite.h>
#include <mw/color.h>

#include <vector>
#include <utility>

namespace zombie {

	// Uses sprites to create a animation.
	class Animation {
	public:
		Animation() {
			index_ = 0;
			reset_ = false;
			time_ = 0;
			lastTime_ = 0;
			speed_ = 1.f;
		}

		// The animation is reset to the first frame.
		void restart() {
			index_ = 0;
			reset_ = true;
		}

		// Add a frame and point it to the current sprite sheet.
		void add(const mw::Sprite& sprite, float bodyWidth, float time = 1.f) {
			frames_.push_back(Frame(sprite, bodyWidth, time));
		}

		void draw(float deltaTime) {
			time_ += deltaTime * speed_;
			if (reset_) {
				reset_ = false;
				lastTime_ = time_;
			}

			if (!frames_.empty()) {
				// New frame?
				if (time_ > frames_[index_].time_ + lastTime_) {
					index_ = (1 + index_) % frames_.size();
					lastTime_ = time_; // Frame updated.
				}

				Frame& frame = frames_[index_];

				glPushMatrix();
				glScalef(frame.sprite_.getWidth() / frame.bodyWidth_, frame.sprite_.getHeight() / frame.bodyWidth_, 1);
				frame.sprite_.draw();
				glPopMatrix();
			}
		}

		void setSpeed(float speed) {
			speed_ = speed;
		}

		float getSpeed() const {
			return speed_;
		}

	private:
		struct Frame {
			Frame() {
			}

			Frame(mw::Sprite sprite, float bodyWidth, float time) : sprite_(sprite), bodyWidth_(bodyWidth), time_(time) {
			}
			
			mw::Sprite sprite_;
			float time_;
			float bodyWidth_;
		};
		
		float lastTime_;
		bool reset_;
		float time_;
		float speed_;

		std::vector<Frame> frames_;
		int index_;
	};

} // Namespace zombie.

#endif // ANIMATION_H
