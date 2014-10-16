#include "missile2d.h"
#include "animation.h"

#include <mw/opengl.h>

namespace zombie {

		Missile2D::Missile2D(const Animation& animation, GameInterface& gameInterface, float width, float length,
			float mass, float speed, float explodeTime, float damage, float explosionRadius)
			: Missile(gameInterface, width, length, mass, speed, explodeTime, damage, explosionRadius),
			animation_(animation) {
		}

		void Missile2D::draw(float accumulator, float timeStep, const GameShader& gameShader) {
			// Draw body.
			const float alpha = accumulator / timeStep;
			
			State state = getState();
			state.position_ = alpha * state.position_ + (1.f - alpha) * previousState().position_;
			gameShader.setGlPositionU(state.position_);
			gameShader.setGlAngleU(getDirection());
			animation_.draw(timeStep, 0, 0, getWidth(), getWidth(), gameShader);
			
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
