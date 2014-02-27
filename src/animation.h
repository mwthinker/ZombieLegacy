#ifndef ANIMATION_H
#define ANIMATION_H

#include "spritesheet.h"

#include <vector>
#include <utility>

namespace zombie {	

	// Uses sprites to create a animation.
	class Animation {
	public:
		Animation() {
			scale_ = 1;
			index_ = 0;
			reset_ = false;
			time_ = 0;
		}

		// All frames are drawn in scaled (scale) pixelsize.
		Animation(const SpriteSheet& spriteSheet, float scale) {
			spriteSheet_ = spriteSheet;
			spriteSheet_.setDrawPixelSize(true);
			index_ = 0;
			scale_ = scale;
			reset_ = false;
			time_ = 0;
		}

		// All frames are drawn in scaled (scale) pixelsize.
		Animation(const mw::TexturePtr& texture, int row, int column, float scale) {
			spriteSheet_ = SpriteSheet(texture, row, column);
			spriteSheet_.setDrawPixelSize(true);
			index_ = 0;
			scale_ = scale;
			reset_ = false;
			time_ = 0;
		}

		// The animation is reset to the first frame.
		void restart() {
			index_ = 0;
			reset_ = true;
		}

		// Add a frame and point it to the current sprite sheet.
		void add(int row, int column, float time = 1.f) {
			frames_.push_back(Frame(row, column, time));
		}

		void setScale(float scale) {
			scale_ = scale;
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
				glScalef(scale_, scale_, scale_);
				glColor3d(1, 1, 1);
				spriteSheet_.draw(frame.row_, frame.column_);
				glPopMatrix();
			}
		}

	private:
		struct Frame {
			Frame() {
			}

			Frame(int row, int column, float time) : row_(row), column_(column), time_(time) {
			}
			
			int row_;
			int column_;
			float time_;
		};

		float scale_;
		float lastTime_;
		bool reset_;
		float time_;

		SpriteSheet spriteSheet_;
		std::vector<Frame> frames_;
		int index_;
	};

} // Namespace zombie.

#endif // ANIMATION_H
