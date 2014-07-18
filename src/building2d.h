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
			/*
			
			glColor3d(0.2, 0.3, 0.9);
			glBegin(GL_POLYGON);

			for (unsigned int i = 0; i < 3 && i < corners.size(); ++i) {
				glVertex2d(corners[i].x, corners[i].y);
			}
			glEnd();
			
			Position delta(0, 0);
			for (unsigned int i = 0; i < 3 && i < corners.size(); ++i) {
				delta += 1.f / corners.size() * corners[i];
			}
			glPushMatrix();
			glColor3d(0.5, 0.5, 0.5);
			glTranslate2f(delta);
			glScalef(1, 1, 1);
			glTranslate2f(-delta);
			*/
			// DRAW BUILDING FOOTPRINT (USE THIS FOR PAVEMENT!)
			glBegin(GL_POLYGON);

			for (unsigned int i = 0; i < 4 && i < corners.size(); ++i) {
				glVertex2d(corners[i].x, corners[i].y);
			}
			glEnd(); 
			// glPopMatrix();

			// DRAW FAKE 3D BUILDING WITH HEIGHT = z 
			// (WHITOUT CORNER OUTLINES)
			int z = 3;
			for (unsigned int i = 0; i < 3 && i < corners.size(); ++i) {
				glBegin(GL_POLYGON);
				glVertex2d(corners[i].x, corners[i].y);
				glVertex2d(corners[i+1].x, corners[i+1].y);
				glVertex2d(corners[i + 1].x, corners[i + 1].y+z);
				glVertex2d(corners[i].x, corners[i].y+z);
				glVertex2d(corners[i].x, corners[i].y);
				glEnd();
			}
			
			
			

		}
	};

} // Namespace zombie.

#endif // BUILDING2D_H
