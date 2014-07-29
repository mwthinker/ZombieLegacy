#ifndef SHOT_H
#define SHOT_H

#include "box2ddef.h"
#include "graphic.h"
#include "bullet.h"
#include "auxiliary.h"

#include <mw/opengl.h>

namespace zombie {

	class Shot : public Graphic {
	public:
		Shot(const Bullet& bullet, Position endPoint) {
			speed_ = 25.f;
			time_ = 0;
			position_ = bullet.postion_;
			direction_ = (endPoint - bullet.postion_);
			float dist = direction_.Normalize();
			duration_ = dist / speed_;
			angle_ = calculateAnglePointToPoint(bullet.postion_, endPoint) * 180 / PI; // RAD -> DEG.
		}

		void draw(float deltaTime) override {
            time_ += deltaTime;
			position_ += deltaTime * speed_ * direction_;

            glPushMatrix();
            glColor3d(1,1,1);
			glTranslate2f(position_);
			glRotatef(angle_);

            glBegin(GL_QUADS);
			glVertex2f(0, -0.02f);
			glVertex2f(0.1f, -0.02f);
			glVertex2f(0.1f, 0.02f);
			glVertex2f(0, 0.02f);
            glEnd();
            glPopMatrix();
        }

		bool toBeRemoved() const override {
            return time_ > duration_;
		}

	private:
		float time_;
		Position position_;
		Position direction_;
		float angle_;
		float duration_;
		float speed_;
	};

} // Namespace zombie.

#endif // SHOT_H

