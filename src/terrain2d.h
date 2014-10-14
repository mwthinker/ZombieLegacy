#ifndef TERRAIN2D_H
#define TERRAIN2D_H

#include "box2ddef.h"

#include <mw/opengl.h>
#include <gui/windowmatrix.h>

#include <vector>

namespace zombie {

	class Terrain2D {
	public:
		Terrain2D() {
		}

		void addRoad(const std::vector<Position>& road) {
			roads_.push_back(road);
		}

		void addWater(const std::vector<Position>& water) {
			lakes_.push_back(water);
		}

		void draw(float time) {
			/*
			glBegin(GL_TRIANGLES);

			glColor3d(0.1, 0.1, 0.1);
			for (const Road& road : roads_) {
				glColor3d(0.1, 0.1, 0.1);
				for (unsigned int i = 0; i < 3 && i < road.size(); ++i) {
					glVertex2d(road[i].x, road[i].y);
				}
			}

			glColor3d(0, 0, 0.3);
			for (const Water& water : lakes_) {
				for (unsigned int i = 0; i < 3 && i < water.size(); ++i) {
					glVertex2d(water[i].x, water[i].y);
				}
			}

			glEnd();
			*/
		}

	private:
		typedef std::vector<Position> Road;
		typedef std::vector<Position> Water;

		std::vector<Road> roads_;
		std::vector<Water> lakes_;
	};

} // Namespace zombie.

#endif // TERRAIN2D_H
