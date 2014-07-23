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
			height_ = 2 + random() * 3;
		}

		void draw(float accumulator, float timeStep) override {
			auto& corners = getCorners();
			// Draw building footprint (Use this for the pavement!)
			glColor3f(0.3f, 0.3f, 0.3f);
			glBegin(GL_POLYGON);
			for (unsigned int i = 0; i < 4 && i < corners.size(); ++i) {
				glVertex2d(corners[i].x, corners[i].y);
			}
			glEnd();
			
			// Draw fake 3D building with height (Without corner outlines).
			for (unsigned int i = 0; i < 3 && i < corners.size(); ++i) {
				glBegin(GL_POLYGON);
				glVertex2d(corners[i].x, corners[i].y);
				glVertex2d(corners[i + 1].x, corners[i + 1].y);
				glVertex2d(corners[i + 1].x, corners[i + 1].y + height_);
				glVertex2d(corners[i].x, corners[i].y + height_);
				glVertex2d(corners[i].x, corners[i].y);
				glEnd();
			}
			
			// Draw fake 3D building outlines with height.
			glColor3f(0.5f, 0.5f, 0.5f);
			glBegin(GL_LINES);
			glVertex2d(corners[0].x, corners[0].y);
			glVertex2d(corners[1].x, corners[1].y);
			
			glVertex2d(corners[0].x, corners[0].y);
			glVertex2d(corners[3].x, corners[3].y);
			
			glVertex2d(corners[0].x, corners[0].y + height_);
			glVertex2d(corners[1].x, corners[1].y + height_);
			
			glVertex2d(corners[0].x, corners[0].y + height_);
			glVertex2d(corners[3].x, corners[3].y + height_);
			
			glVertex2d(corners[3].x, corners[3].y);
			glVertex2d(corners[3].x, corners[3].y + height_);
			
			glVertex2d(corners[0].x, corners[0].y);
			glVertex2d(corners[0].x, corners[0].y + height_);
			
			glVertex2d(corners[1].x, corners[1].y);
			glVertex2d(corners[1].x, corners[1].y + height_);
			
			glVertex2d(corners[3].x, corners[3].y + height_);
			glVertex2d(corners[2].x, corners[2].y + height_);
			
			glVertex2d(corners[1].x, corners[1].y + height_);
			glVertex2d(corners[2].x, corners[2].y + height_);
			glEnd();
		}
	
	private:
		float height_;
	};



} // Namespace zombie.

#endif // BUILDING2D_H
