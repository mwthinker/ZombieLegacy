#ifndef SPRITESHEET_H
#define SPRITESHEET_H

#include <mw/texture.h>

#include <cassert>

namespace zombie {

	class SpriteSheet {
	public:
		SpriteSheet() : rows_(0), columns_(0), drawPixelSize_(false) {
		}

		SpriteSheet(const mw::TexturePtr& texture, int rows, int columns) : texture_(texture), rows_(rows), columns_(columns), drawPixelSize_(false) {
			// Must be valid texture!
			assert(texture);
		}

		// Draws the choosen image on the spritesheet. The upper left "image" 
		// corrensponds to row = 0 and column = 0.
		void draw(int row, int column) {
			// Must be inside the sprite sheet.
			assert(row >= 0 && row < rows_ && column >= 0 && column < columns_);
			// Valid texture?
			if (texture_ != 0 && texture_->isValid()) {
				texture_->bind();

				glEnable(GL_BLEND);
				glEnable(GL_TEXTURE_2D);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

				float dx = 1.f / columns_;
				float dy = 1.f / rows_;
				float x = column * dx;
				float y = row * dy;

				glPushMatrix();

				if (drawPixelSize_) {
					glScaled(texture_->getWidth() * dx, texture_->getHeight() * dy, 1);
				}

				// Inverts the image. Y-is "up" for the window and x is "right".
				glScaled(1, -1, 1);

				glNormal3f(0, 0, 1);
				glBegin(GL_QUADS);
				glTexCoord2f(x, y);
				glVertex2f(-0.5f, -0.5f);

				glTexCoord2f(x + dx, y);
				glVertex2f(0.5f, -0.5f);

				glTexCoord2f(x + dx, y + dy);
				glVertex2f(0.5f, 0.5f);

				glTexCoord2f(x, y + dy);
				glVertex2f(-0.5f, 0.5f);
				glEnd();

				glPopMatrix();

				glDisable(GL_TEXTURE_2D);
				glDisable(GL_BLEND);
			}
		}

		inline int getRows() const {
			return rows_;
		}

		inline int getColumns() const {
			return columns_;
		}

		const mw::TexturePtr& getTexture() const {
			return texture_;
		}

		void setDrawPixelSize(bool drawPixelSize) {
			drawPixelSize_ = drawPixelSize;
		}

		bool isDrawingPixelSize() const {
			return drawPixelSize_;
		}

	private:
		int rows_, columns_;
		mw::TexturePtr texture_;

		bool drawPixelSize_;
	};

} // Namespace zombie.

#endif // SPRITESHEET_H
