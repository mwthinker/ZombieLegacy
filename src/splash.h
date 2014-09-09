#ifndef SPLASH_H
#define SPLASH_H

#include "box2ddef.h"
#include "auxiliary.h"
#include "particleengine.h"

#include <mw/opengl.h>
#include <mw/color.h>
#include <mw/sprite.h>

namespace zombie {

	class Splash {
	public:
		Splash() : particleEngine_(*this, mw::Texture("images/splash.png")) { //, 0.f, 0.f, 0.f, 1.f, 0.2f, 0.6f, 0.1f, 0.3f) {
			particleEngine_.setLoop(false);
			//particleEngine_.setColor(0.1f, 0.1f, 0.1f);
		}

		void draw(float deltaTime, const mw::Matrix44& matrix) {
			particleEngine_.draw(deltaTime, matrix);
		}

		void setPosition(Position position) {
			particleEngine_.setPosition(position);
		}

		void update(float deltaTime, Particle& particle) {
			particle.lifeTime_ += deltaTime;
			particle.pos_ += deltaTime * particle.vel_;
		}

		void restart() {
			particleEngine_.restart();
		}

		void init(Particle& particle) {

		}

		ParticleEngine<300, Splash> particleEngine_;
	};

} // Namespace zombie.s

#endif // SPLASH_H
