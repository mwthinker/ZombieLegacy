#include "map2d.h"
#include "gamesprite.h"

#include <SDL_opengl.h>

namespace zombie {

	Map2D::Map2D() {
	}

	Map2D::Map2D(float minX, float maxX, float minY, float maxY) {
		grass_ = grassSprite;
		minX_ = minX;
		maxX_ = maxX;
		minY_ = minY;
		maxY_ = maxY;
	}

	void Map2D::draw(float time) {
		glColor3d(0.6,0.6,0.6);
		grassTexture->bind();
		
		double tH = 1.0;
		double tW = 1.0;

		glEnable(GL_TEXTURE_2D);
		glBegin(GL_QUADS);
		for (double x = minX_; x < maxX_; x += 10.0) {
			for (double y = minY_; y < maxY_; y += 10.0) {
				glTexCoord2d(0, 0);
				glVertex2d(x, y);
				glTexCoord2d(tW, 0);
				glVertex2d(x+10, y);
				glTexCoord2d(tW, tH);
				glVertex2d(x+10, y+10);
				glTexCoord2d(0, tH);
				glVertex2d(x, y+10);
			}
		}
		glEnd();
		glDisable(GL_TEXTURE_2D);
	}

} // Namespace zombie.
