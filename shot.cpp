#include "shot.h"

#include <SDL_opengl.h>

using namespace zombie;

Shot::Shot(Position start, Position end, float currentTime) {
	startTime_ = currentTime;
	start_ = start;
	end_ = end;
}

bool Shot::draw(double time) {
	if (time < startTime_ + 2) {
		// Draw view sphere
		glColor3d(1,1,1);
		glBegin(GL_LINES);
		glVertex2d(start_.x,start_.y);
		glVertex2d(end_.x,end_.y);
		glEnd();
		return true;
	} else {
		return false;
	}
}
