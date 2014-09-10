#ifndef CAR2D_H
#define CAR2D_H

#include "car.h"
#include "auxiliary.h"
#include "animation.h"

#include <mw/opengl.h>

namespace zombie {

	class Car2D : public Car {
	public:
		Car2D(float mass, float life, float width, float length, const Animation& animation) :
			Car(mass, life, width, length), animation_(animation) {
		}

		// Draws the car.
		void draw(float accumulator, float timeStep, gui::WindowMatrixPtr wPtr) override {
			// Draw body.
			const float alpha = accumulator / timeStep;

			State state = getState();
			state.position_ = alpha * state.position_ + (1.f - alpha) * previousState().position_;

			wPtr->useShader();
			wPtr->setColor(1, 1, 1);
			mw::Matrix44 old = wPtr->getModel();
			wPtr->setModel(old * mw::getTranslateMatrix(state.position_.x, state.position_.y) * mw::getRotateMatrix(state.angle_, 0, 0, 1) * mw::getScaleMatrix(getWidth(), getWidth()));
			animation_.draw(timeStep, wPtr);
			wPtr->setModel(old);
		}

	private:
		Animation animation_;
		State previousState_;
	};
	
} // Namespace zombie.

#endif // CAR2D_H
