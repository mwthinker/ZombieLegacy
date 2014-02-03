#ifndef BUILDING2D_H
#define BUILDING2D_H

#include "building.h"

#include <SDL_opengl.h>

#include <vector>

namespace zombie {

	class Building2D : public Building {
	public:
		Building2D(const std::vector<Position>& corners) : Building(corners) {
		}

		void draw(float time) override {
			auto& corners = getCorners();
			
			glColor3d(0.5, 0.5, 0.5);
			glBegin(GL_TRIANGLES);
			for (unsigned int i = 0; i < 3 && i < corners.size(); ++i) {
				glVertex2d(corners[i].x, corners[i].y);
			}
			glEnd();
		}
	};

} // Namespace zombie.

#endif // BUILDING2D_H
