#ifndef BUILDINGDRAW_H
#define BUILDINGDRAW_H

#include "task.h"
#include "typedefs.h"

#include <SDL_opengl.h>

#include <vector>

namespace zombie {

	class BuildingDraw : public Task {
	public:
		BuildingDraw(const std::vector<Position>& corners) : corners_(corners) {
		}

		bool update(float time) override {
			glColor3d(0.9,0.7,0.7);
			glBegin(GL_QUADS);
			for (unsigned int i = 0; i < 4 && i < corners_.size(); ++i) {
				glVertex2d(corners_[i].x, corners_[i].y);
			}
			glEnd();
			return true;
		}

	private:
		std::vector<Position> corners_;
	};

} // Namespace zombie.

#endif // BUILDINGDRAW_H
