#include "explosion.h"
#include "box2ddef.h"
#include "auxiliary.h"

#include <mw/opengl.h>
#include <mw/color.h>
#include <mw/sprite.h>
#include <mw/sound.h>

namespace zombie {

	Explosion::Explosion() {
	}

	void Explosion::draw(float deltaTime, const GameShader& shader) {
		sound_.play();
		
		float diameter = 2 * blastRadius_;
		float angle = 0;

		shader.useProgram();
		shader.setGlColorU(1, 1, 1);
		shader.setGlGlobalPositionU(position_);
		shader.setGlLocalAngleU(angle);
		animation_.draw(deltaTime, 0, 0, diameter, diameter, shader);
	}	

	bool Explosion::toBeRemoved() const {
		return animation_.isEnded();
	}

	void Explosion::restart(Position position, float blastRadius, const ExplosionProperties& exPr) {
		position_ = position;
		delay_ = exPr.delay_;
		sound_ = exPr.sound_;
		blastRadius_ = blastRadius;
		animation_ = exPr.animation_;
	}

} // Namespace zombie.
