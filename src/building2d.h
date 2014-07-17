#ifndef BUILDING2D_H
#define BUILDING2D_H

#include "building.h"
#include "auxiliary.h"

#include <SDL_opengl.h>

#include <vector>

namespace zombie {

	class Building2D : public Building {
	public:
		Building2D(const std::vector<Position>& corners) : Building(corners) {
		}

		void draw(float time) override {
			auto& corners = getCorners();
			Position delta(0, 0);
			for (unsigned int i = 0; i < 3 && i < corners.size(); ++i) {
				delta += 1.f / corners.size() * corners[i];
			}
			glPushMatrix();
			glColor3d(0.5, 0.5, 0.5);
			glTranslate2f(delta);
			glScalef(1, 1, 1);
			glTranslate2f(-delta);
			glBegin(GL_TRIANGLES);

			for (unsigned int i = 0; i < 3 && i < corners.size(); ++i) {
				glVertex2d(corners[i].x, corners[i].y);
			}
			glEnd();
			glPopMatrix();
		}
	};

} // Namespace zombie.

#endif // BUILDING2D_H
