#include "animation.h"
#include "auxiliary.h"

#include <mw/sprite.h>

#include <vector>

namespace zombie {

	Animation::Animation() {
		index_ = 0;
		reset_ = false;
		time_ = 0;
		lastTime_ = 0;
		speed_ = 1.f;
		loop_ = true;
		end_ = false;
	}

	// The animation is reset to the first frame.
	void Animation::restart() {
		index_ = 0;
		reset_ = true;
		end_ = false;
	}

	// Add a frame and point it to the current sprite sheet.
	void Animation::add(const mw::Sprite& sprite, float bodyWidth, float time) {
		frames_.push_back(Frame(sprite, bodyWidth, time));
	}

	void Animation::draw(float deltaTime) {
		time_ += deltaTime * speed_;
		if (reset_) {
			reset_ = false;
			lastTime_ = time_;
		}

		if (!frames_.empty()) {
			// New frame?
			if (!end_ && time_ > frames_[index_].time_ + lastTime_) {
				if (loop_) {
					index_ = (1 + index_) % frames_.size();
				} else {
					if (index_ + 1 < frames_.size()) {
						++index_;
					} else {
						end_ = true;
					}
				}
				lastTime_ = time_; // Frame updated.
			}

			Frame& frame = frames_[index_];

			glPushMatrix();
			glScalef(frame.sprite_.getWidth() / frame.bodyWidth_, frame.sprite_.getHeight() / frame.bodyWidth_, 1);
			frame.sprite_.draw();
			glPopMatrix();
		}
	}

} // Namespace zombie.
