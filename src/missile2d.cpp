#include "missile2d.h"
#include "animation.h"

#include <mw/opengl.h>

namespace zombie {

	Missile2D::Missile2D(const MissileProperties& properties, GameInterface* gameInterface) :
		Missile(gameInterface, properties.width_, properties.length_, properties.mass_,
			properties.speed_, properties.explosionTime_, properties.damage_,
			properties.explosionRadius_),
		
		animation_(properties.animation_) {

	}

	void Missile2D::draw(float accumulator, float timeStep, const GameShader& gameShader) {
		// Draw body.
		const float alpha = accumulator / timeStep;
			
		State state = getState();
		state.position_ = alpha * state.position_ + (1.f - alpha) * previousState().position_;
		gameShader.useGlShader();
		gameShader.setGlPositionU(state.position_);
		gameShader.setGlAngleU(getDirection());
		gameShader.setGlColorU(1, 1, 1);
		animation_.draw(timeStep, 0, 0, getWidth(), getWidth(), gameShader);
	}

	MissileProperties loadMissileProperties(GameDataEntry& entry) {
		float mass = entry.getChildEntry("range").getFloat();
		float width = entry.getChildEntry("width").getFloat();
		float length = entry.getChildEntry("length").getFloat();
		Animation animation = entry.getChildEntry("animation").getAnimation();
		mw::Sound moveSound = entry.getChildEntry("moveSound").getSound();
		float damageRadius = entry.getChildEntry("damageRadius").getFloat();
		float damage = entry.getChildEntry("damage").getFloat();;
		float deathTime = entry.getChildEntry("deathTime").getFloat();
		float speed = entry.getChildEntry("speed").getFloat();

		return MissileProperties(width, length, mass, speed, deathTime, damageRadius, damage);
	}

} // Namespace zombie.
