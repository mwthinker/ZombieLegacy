#include "missile2d.h"
#include "animation.h"

#include <mw/opengl.h>

namespace zombie {

		Missile2D::Missile2D(const Animation& animation, GameInterface& gameInterface, float width, float length,
			float mass, float speed, float explodeTime, float damage, float explosionRadius)
			: Missile(gameInterface, width, length, mass, speed, explodeTime, damage, explosionRadius),
			animation_(animation) {
		}

		void Missile2D::draw(float accumulator, float timeStep, gui::WindowMatrixPtr wPtr) {
			// Draw body.
			const float alpha = accumulator / timeStep;

			State state = getState();
			state.position_ = alpha * state.position_ + (1.f - alpha) * previousState().position_;

			wPtr->useShader();
			wPtr->setColor(1, 1, 1);
			mw::Matrix44 old = wPtr->getModel();
			wPtr->setModel(old * mw::getTranslateMatrix44(state.position_.x, state.position_.y) * mw::getRotateMatrix44(state.angle_, 0, 0, 1) * mw::getScaleMatrix44(getWidth(), getWidth()));
			animation_.draw(timeStep, wPtr);
			wPtr->setModel(old);
		}

	Missile2D loadMissile2D(GameInterface* gameInterface, GameDataEntry& entry, float damage, float range) {
		float mass = entry.getChildEntry("range").getFloat();
		float width = entry.getChildEntry("width").getFloat();
		float length = entry.getChildEntry("length").getFloat();
		Animation animation = entry.getChildEntry("animation").getAnimation();
		mw::Sound moveSound = entry.getChildEntry("moveSound").getSound();
		float damageRadius = entry.getChildEntry("damageRadius").getFloat();
		float deathTime = entry.getChildEntry("deathTime").getFloat();
		float speed = entry.getChildEntry("speed").getFloat();

		return Missile2D(animation, *gameInterface, width, length, mass, speed, deathTime, damage, damageRadius);
	}

} // Namespace zombie.
