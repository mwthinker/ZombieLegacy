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
		gameShader.useProgram();
		gameShader.setGlGlobalPositionU(state.position_);
		gameShader.setGlLocalAngleU(state.angle_);
		gameShader.setGlColorU(1, 1, 1);
		animation_.draw(timeStep, 0, 0, getLength(), getLength(), gameShader);
	}

	Missile2D loadMissile2D(GameInterface* gameInterface, const MissileProperties& mp) {
		return Missile2D(gameInterface, mp.width_, mp.length_, mp.mass_, mp.animation_, mp.moveSound_);
	}

} // Namespace zombie.
