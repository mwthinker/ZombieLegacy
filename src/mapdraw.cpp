#include "mapdraw.h"
#include "map.h"

#include <SDL_opengl.h>

namespace zombie {

	MapDraw::MapDraw(const Map& map) : map_(map) {
		grass_ = grassSprite;
	}

	bool MapDraw::update(double time) {
		glColor3d(0.6,0.6,0.6);
		grassTexture->bind();		

		double maxX = map_.maxX();
		double maxY = map_.maxY();
		double tH = 1.0;
		double tW = 1.0;

		glEnable(GL_TEXTURE_2D);
		glBegin(GL_QUADS);
		for (double x = map_.minX(); x < maxX; x += 10.0) {
			for (double y = map_.minY(); y < map_.maxY(); y += 10.0) {
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
		return true;
	}

} // Namespace zombie.
