#ifndef SHOT_H
#define SHOT_H

#include "box2ddef.h"
#include "graphic.h"
#include "auxiliary.h"

#include <mw/opengl.h>

namespace zombie {

	class Shot : public Graphic {
	public:
		Shot(Position startPoint, Position endPoint) {
			speed_ = 25.f;
			time_ = 0;
			position_ = startPoint;
			direction_ = (endPoint - startPoint);
			float dist = direction_.Normalize();
			duration_ = dist / speed_;
			angle_ = calculateAnglePointToPoint(startPoint, endPoint) * 180 / PI; // RAD -> DEG.
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

