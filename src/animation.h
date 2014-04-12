#ifndef ANIMATION_H
#define ANIMATION_H

#include <mw/sprite.h>

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
		}

		// The animation is reset to the first frame.
		void restart() {
			index_ = 0;
			reset_ = true;
		}

		// Add a frame and point it to the current sprite sheet.
		void add(const mw::Sprite& sprite, float scale, float time = 1.f) {
			frames_.push_back(Frame(sprite, scale, time));
		}

		// Draws the correct frame at the (time) time.
		void draw(float deltaTime) {
			time_ += deltaTime;
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
				//glTranslatef(0.5f, 0.5f, 0);
				glScalef(frame.sprite_.getWidth(), frame.sprite_.getHeight(), 1);
				glScalef(frame.scale_, frame.scale_, frame.scale_);
				glColor3d(1, 1, 1);
				frame.sprite_.draw();
				glPopMatrix();
			}
		}

	private:
		struct Frame {
			Frame() {
			}

			Frame(mw::Sprite sprite, float scale, float time) : sprite_(sprite), scale_(scale), time_(time) {
			}
			
			mw::Sprite sprite_;
			float time_;
			float scale_;
		};
		
		float lastTime_;
		bool reset_;
		float time_;

		std::vector<Frame> frames_;
		int index_;
	};

} // Namespace zombie.

#endif // ANIMATION_H
