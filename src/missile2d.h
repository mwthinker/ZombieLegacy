#ifndef MISSILE2D_H
#define MISSILE2D_H

#include "missile.h"
#include "animation.h"

#include <mw/opengl.h>

namespace zombie {	

	class Missile2D : public Missile {
	public:
		Missile2D(const Animation& animation, GameInterface& gameInterface, float width, float length,
			float speed, float explodeTime, float damage, float explosionRadius)
			: Missile(gameInterface, width, length, speed, explodeTime, damage, explosionRadius),
			animation_(animation) {
		}

		void draw(float accumulator, float timeStep) override {
			// Draw body.
			const float alpha = accumulator / timeStep;

			State state = getState();
			state.position_ = alpha * state.position_ + (1.f - alpha) * previousState().position_;

			glPushMatrix();
			glTranslate2f(state.position_);
			glRotated(state.angle_ * 180 / PI, 0, 0, 1);
			glScaled(getWidth(), getWidth(), 1);
			animation_.draw(timeStep);
			glPopMatrix();
		}

	private:
		Animation animation_;
		State previousState_;
	};

} // Namespace zombie.

#endif // MISSILE_H
