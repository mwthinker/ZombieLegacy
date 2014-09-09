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

		void draw(float deltaTime, gui::WindowMatrixPtr wPtr) override {
			wPtr->useShader();
			wPtr->setColor(1, 1, 1);
			mw::Matrix44 old = wPtr->getModel();
			wPtr->setModel(old * mw::getTranslateMatrix(position_.x, position_.y) * mw::getRotateMatrix(angle_, 0, 0, 1));
			//glScale2f(2 * getRadius());
			animation_.draw(deltaTime, wPtr);
			wPtr->setModel(old);
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
