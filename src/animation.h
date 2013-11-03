#ifndef ANIMATION_H
#define ANIMATION_H

#include <mw/sprite.h>

#include <vector>
#include <utility>
#include <tuple>

namespace zombie {

	typedef std::tuple<std::string, float, float> TupleImageScaleTime;
		
	// Uses sprites to create a animation.
	class Animation {
	public:
		Animation() {
			scale_ = 1.f;
			index_ = 0;
			reset_ = false;
		}

		// All frames are drawn in scaled (scale) pixelsize.
		Animation(float scale) {
			index_ = 0;
			scale_ = scale;
			reset_ = false;
		}

		// The animation is reset to the first frame.
		void restart() {
			index_ = 0;
			reset_ = true;
		}

		// Add the frame (sprite) with duration (time).
		void add(mw::Sprite sprite, float time = 1.f) {
			sprite.setDrawPixelSize(true);
			sprites_.push_back(Pair(sprite, time));			
		}

		void setScale(float scale) {
			scale_ = scale;
		}

		// Draws the correct frame at the (time) time.
		void draw(float time) {
			if (reset_) {
				reset_ = false;
				lastTime_ = time;
			}

			if (!sprites_.empty()) {
				// New frame?
				if (time > sprites_[index_].second + lastTime_) {
					index_ = (1 + index_) % sprites_.size();
					lastTime_ = time; // Frame updated.					
				}

				glPushMatrix();
				glScalef(scale_, scale_, scale_);
				glColor3d(1, 1, 1);
				sprites_[index_].first.draw();
				glPopMatrix();
			}
		}

	private:
		float scale_;
		float lastTime_;
		bool reset_;

		typedef std::pair<mw::Sprite, float> Pair;
		std::vector<Pair> sprites_;
		int index_;
	};

} // Namespace zombie.

#endif // ANIMATION_H
