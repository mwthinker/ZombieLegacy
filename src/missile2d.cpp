#include "missile2d.h"
#include "animation.h"

#include <mw/opengl.h>

namespace zombie {

	Missile2D::Missile2D(GameInterface* gameInterface, float width, float length, float mass,
		const Animation& animation, const mw::Sound& moveSound) :
		Missile(gameInterface, width, length, mass),
		animation_(animation) {

	}

	void Missile2D::draw(float accumulator, float timeStep, const GameShader& gameShader) {
		// Draw body.
		const float alpha = accumulator / timeStep;
			
		State state = getState();
		state.position_ = alpha * state.position_ + (1.f - alpha) * previousState().position_;
		gameShader.glUseProgram();
		gameShader.setGlGlobalPositionU(state.position_);
		gameShader.setGlLocalAngleU(state.angle_);
		gameShader.setGlColorU(1, 1, 1);
		animation_.draw(timeStep, 0, 0, getLength(), getLength(), gameShader);
	}

	Missile2D loadMissile2D(GameInterface* gameInterface, ZombieEntry& entry) {
		float mass = entry.getChildEntry("mass").getFloat();
		float width = entry.getChildEntry("width").getFloat();
		float length = entry.getChildEntry("length").getFloat();
		Animation animation = entry.getChildEntry("animation").getAnimation();
		mw::Sound moveSound = entry.getChildEntry("moveSound").getSound();

		return Missile2D(gameInterface, width, length, mass, animation, moveSound);
	}

} // Namespace zombie.
