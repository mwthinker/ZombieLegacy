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
			scale_ = 1.f;
		}

		// All frames are drawn in scaled (scale) pixelsize.
		Animation(float scale) {
			index_ = 0;
		}

		// The animation is reset to the first frame.
		void restart() {
			index_ = 0;
		}

		// Add the frame (sprite) with duration (time).
		void add(mw::Sprite sprite, float time = 1.f) {
			sprites_.push_back(Pair(sprite, time));
			sprite.setDrawPixelSize(true);
		}

		// Draws the correct frame at the (time) time.
		void draw(float time) {
			if (!sprites_.empty()) {
				// New frame?
				if (time > sprites_[index_].second + lastTime_) {
					index_ = (1 + index_) % sprites_.size();
					lastTime_ = time; // Frame updated.
				}

				glPushMatrix();
				glScalef(scale_, scale_, scale_);
				sprites_[index_].first.draw();
				glPopMatrix();
			}
		}

	private:
		float scale_;
		float lastTime_;

		typedef std::pair<mw::Sprite, float> Pair;
		std::vector<Pair> sprites_;
		int index_;
	};

} // Namespace zombie.

#endif // ANIMATION_H
