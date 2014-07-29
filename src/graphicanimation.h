#ifndef GRAPHICANIMATION_H
#define GRAPHICANIMATION_H

#include "box2ddef.h"
#include "graphic.h"
#include "animation.h"
#include "auxiliary.h"

#include <mw/opengl.h>

namespace zombie {

	class GraphicAnimation : public Graphic {
	public:
		GraphicAnimation(Position position, float angle, const Animation& animation) : animation_(animation) {
			position_ = position;
			angle_ = angle;
			animation_.setLooping(false);
		}

		void draw(float deltaTime) override {
			glPushMatrix();
			glTranslate2f(position_);
			//glScale2f(2 * getRadius());
			glRotated(angle_ * 180 / PI, 0, 0, 1);
			glColor3d(1, 1, 1);
			animation_.draw(deltaTime);
			glPopMatrix();
		}

		bool toBeRemoved() const override {
			return animation_.isEnded();
		}
	
	private:
		Animation animation_;
		Position position_;
		float angle_;
	};

} // Namespace zombie.

#endif // GRAPHICANIMATION_H
