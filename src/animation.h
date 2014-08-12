#ifndef ANIMATION_H
#define ANIMATION_H

#include "box2ddef.h"
#include "auxiliary.h"

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
			loop_ = true;
			end_ = false;
		}

		// The animation is reset to the first frame.
		void restart() {
			index_ = 0;
			reset_ = true;
			end_ = false;
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

		void setSpeed(float speed) {
			speed_ = speed;
		}

		float getSpeed() const {
			return speed_;
		}

		void setLooping(bool loop) {
			loop_ = loop;
		}

		bool isLooping() const {
			return loop_;
		}

		bool isEnded() const {
			return end_;
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
		unsigned int index_;
		bool loop_;
		bool end_;
	};

} // Namespace zombie.

#endif // ANIMATION_H
