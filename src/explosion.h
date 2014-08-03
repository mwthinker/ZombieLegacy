#ifndef EXPLOSION_H
#define EXPLOSION_H

#include "box2ddef.h"
#include "auxiliary.h"
#include "graphic.h"
#include "particleengine.h"
#include "shockwave.h"
#include "emitter.h"

#include <mw/opengl.h>
#include <mw/color.h>
#include <mw/sprite.h>
#include <mw/sound.h>

#include <array>
#include <vector>

namespace zombie {

	class Explosion : public Graphic {
	public:
		Explosion(const mw::Texture& particle, const mw::Sprite& shockwave, const mw::Sprite& emitter, const mw::Sound& sound)
			: shockWave_(shockwave, 5, 1 / 2.f),
			particleEngine_(particle, 0.f, 0.f, 0.f, 1.f, 0.1f, 0.6f, 0.1f, 0.3f),			
			emitter_(emitter, 10.f, 30.f, 0.2f) {

			particleEngine_.setLoop(false);
			time_ = 0;
			delay_ = 0.1f;
			position_ = ZERO;
			angle_ = 0.f;
			sound_ = sound;
		}

		Explosion(const Position position, float angle, const Explosion& explosion) : particleEngine_(explosion.particleEngine_), shockWave_(explosion.shockWave_), emitter_(explosion.emitter_) {
			position_ = position;
			angle_ = angle;
			time_ = explosion.time_;
			delay_ = explosion.delay_;
			sound_ = explosion.sound_;
		}		

		void draw(float deltaTime) override {
			time_ += deltaTime;
			glPushMatrix();
			glTranslate2f(position_);
			glRotatef(angle_);
			shockWave_.draw(deltaTime);
			//emitter_.draw(deltaTime);
			if (time_ > delay_) {
				particleEngine_.draw(deltaTime);
			}
			glPopMatrix();
		}

		bool toBeRemoved() const {
			return particleEngine_.isFinnish();
		}

	private:
		Position position_;
		float angle_;
		ParticleEngine<1000> particleEngine_;
		ShockWave shockWave_;
		Emitter<100> emitter_;
		float delay_;
		float time_;
		mw::Sound sound_;
	};

} // Namespace zombie.

#endif // EXPLOSION_H
