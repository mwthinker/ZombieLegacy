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
		frames_.emplaceBack(sprite, bodyWidth, time);
	}

	void Animation::draw(float deltaTime, float x, float y, float w, float h, const GameShader& shader) {
		time_ += deltaTime * speed_;
		if (reset_) {
			reset_ = false;
			lastTime_ = time_;
		}

		if (frames_.getSize() > 0) {
			// New frame?
			if (!end_ && time_ > frames_[index_].time_ + lastTime_) {
				if (loop_) {
					index_ = (1 + index_) % frames_.getSize();
				} else {
					if (index_ + 1 < frames_.getSize()) {
						++index_;
					} else {
						end_ = true;
					}
				}
				lastTime_ = time_; // Frame updated.
			}

			Frame& frame = frames_[index_];
			drawSprite(frame.sprite_, shader, x, y, w * frame.sprite_.getWidth() / frame.bodyWidth_, h * frame.sprite_.getHeight() / frame.bodyWidth_);
		}
	}

	void Animation::drawSprite(const mw::Sprite& sprite, const GameShader& shader, float x, float y, float width, float height) const {
		const auto& texture = sprite.getTexture();
		if (texture.isValid()) {
			texture.bindTexture();
			mw::glEnable(GL_BLEND);
			mw::glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			// Centered square in ORIGO.
			GLfloat aVertices[] = {
				x + -0.5f * width, y + -0.5f * height,
				x + 0.5f * width,  y + -0.5f * height,
				x + -0.5f * width, y + 0.5f * height,
				x + 0.5f * width, y + 0.5f * height};

			// Map the sprite out from the texture.
			GLfloat aTexCoord[] = {
				sprite.getX() / texture.getWidth(), sprite.getY() / texture.getHeight(),
				(sprite.getX() + sprite.getWidth()) / texture.getWidth(), sprite.getY() / texture.getHeight(),
				sprite.getX() / texture.getWidth(), (sprite.getY() + sprite.getHeight()) / texture.getHeight(),
				(sprite.getX() + sprite.getWidth()) / texture.getWidth(), (sprite.getY() + sprite.getHeight()) / texture.getHeight()};

			// Use the program object.
			shader.setGlTextureU(true);

			// Load the vertex data.
			shader.setGlVer2dCoordsA(aVertices);
			shader.setGlTexCoordsA(aTexCoord);

			// Upload the attributes and draw the sprite.
			mw::glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
			mw::glDisable(GL_BLEND);
			mw::checkGlError();
		}
	}

} // Namespace zombie.
