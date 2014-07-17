#ifndef SHOT_H
#define SHOT_H

#include "box2ddef.h"
#include "graphic.h"
#include "bullet.h"
#include "auxiliary.h"

#include <SDL_opengl.h>

namespace zombie {

	class Shot : public Graphic {
	public:
		Shot(const Bullet& bullet, Position endPoint) {
            endPoint_ = endPoint;
            startPoint_ = bullet.postion_;
            duration_ = 1;
            time_ = 0;
		}

		void draw(float deltaTime) override {
            time_ += deltaTime;
            glPushMatrix();
            glColor3d(1,1,1);
            glBegin(GL_LINES);
            glVertex2d(startPoint_.x, startPoint_.y);
            glVertex2d(endPoint_.x, endPoint_.y);
            glEnd();
            glPopMatrix();
        }

		bool toBeRemoved() const override {
            return time_ > duration_;
		}

	private:
		float time_;
		Position startPoint_;
		Position endPoint_;
		float duration_;
	};

} // Namespace zombie.

#endif // SHOT_H

