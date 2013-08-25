#ifndef BUILDINGDRAW_H
#define BUILDINGDRAW_H

#include "graphictask.h"
#include "building.h"

#include <SDL_opengl.h>

namespace zombie {

	class BuildingDraw : public GraphicTask {
	public:
		BuildingDraw(Building* building) : building_(building) {
		}

		void draw(double time) override {
			glColor3d(0.9,0.7,0.7);
			auto corners = building_->getCorners();
			glBegin(GL_QUADS);
			for (unsigned int i = 0; i < 4 && i < corners.size(); ++i) {
				glVertex2d(corners[i].x, corners[i].y);
			}
			glEnd();
		}

		bool isRunning() const override {
			return true;
		}

	private:
		Building* building_;
	};

} // Namespace zombie.

#endif // BUILDINGDRAW_H
