#ifndef ANIMATION_H
#define ANIMATION_H

#include "box2ddef.h"
#include "gameshader.h"
#include "array.h"

#include <mw/sprite.h>

namespace zombie {

	// Uses sprites to create a animation.
	class Animation {
	public:
		Animation();

		// The animation is reset to the first frame.
		void restart();

		// Add a frame and point it to the current sprite sheet.
		void add(const mw::Sprite& sprite, float bodyWidth, float time = 1.f);
		
		void draw(float deltaTime, float x, float y, float w, float h, const GameShader& shader);

		inline void setSpeed(float speed) {
			speed_ = speed;
		}

		inline float getSpeed() const {
			return speed_;
		}

		inline void setLooping(bool loop) {
			loop_ = loop;
		}

		inline bool isLooping() const {
			return loop_;
		}

		inline bool isEnded() const {
			return end_;
		}

		struct Frame {
			inline Frame() {
			}

			inline Frame(mw::Sprite sprite, float bodyWidth, float time) : sprite_(sprite), bodyWidth_(bodyWidth), time_(time) {
			}

			mw::Sprite sprite_;
			float time_;
			float bodyWidth_;
		};

	private:
		void drawSprite(const mw::Sprite& sprite, const GameShader& shader, float x, float y, float width, float height) const;
		
		float lastTime_;
		bool reset_;
		float time_;
		float speed_;

		Array<Frame, 10> frames_;
		unsigned int index_;
		bool loop_;
		bool end_;
	};

} // Namespace zombie.

#endif // ANIMATION_H
