#ifndef MISSILE2D_H
#define MISSILE2D_H

#include "missile.h"
#include "animation.h"

#include <mw/opengl.h>

namespace zombie {	

	class Missile2D : public Missile {
	public:
		Missile2D(const Animation& animation, GameInterface& gameInterface, float width, float length,
			float mass, float speed, float explodeTime, float damage, float explosionRadius)
			: Missile(gameInterface, width, length, mass, speed, explodeTime, damage, explosionRadius),
			animation_(animation) {
		}

		void draw(float accumulator, float timeStep) override {
			// Draw body.
			const float alpha = accumulator / timeStep;

			State state = getState();
			state.position_ = alpha * state.position_ + (1.f - alpha) * previousState().position_;

			auto wPtr = getWindowMatrix();
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

#endif // MISSILE_H
