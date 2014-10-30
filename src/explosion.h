#ifndef EXPLOSION_H
#define EXPLOSION_H

#include "box2ddef.h"
#include "auxiliary.h"
#include "particleengine.h"

#include <mw/opengl.h>
#include <mw/color.h>
#include <mw/sound.h>

namespace zombie {

	class GameDataEntry;

	struct ExplosionProperties {
		ExplosionProperties() {
		}

		ExplosionProperties(float blastRadius, float speed, mw::Texture particle,
			mw::Sound sound = mw::Sound(), float delay = 0.3) :

			blastRadius_(blastRadius),
			speed_(blastRadius),
			particle_(particle),
			sound_(sound),
			delay_(delay) {
		}

		float blastRadius_;
		float speed_;
		mw::Texture particle_;
		mw::Sound sound_;
		float delay_;
	};

	class Explosion {
	public:
		Explosion();

		void draw(float deltaTime, const GameShader& shader);

		float getTimeDuration() const;

		bool toBeRemoved() const;

		void restart(Position position, const ExplosionProperties& exPr);

		void update(float deltaTime, Particle& particle);

		void init(Particle& particle);

	private:
		float time_;

		Position position_;
		float speed_;
		float lifeTime_;
		float particleSize_;
		float blastRadius_;

		ParticleEngine<200, Explosion> particleEngine_;
		float delay_;
		mw::Sound sound_;
	};

	ExplosionProperties loadExplosion(GameDataEntry& entry);

} // Namespace zombie.

#endif // EXPLOSION_H
