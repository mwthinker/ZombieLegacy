#ifndef EXPLOSION_H
#define EXPLOSION_H

#include "box2ddef.h"
#include "auxiliary.h"
#include "gameshader.h"
#include "animation.h"

#include <mw/opengl.h>
#include <mw/color.h>
#include <mw/sound.h>
#include <mw/sprite.h>

#include <vector>

namespace zombie {

	class ZombieEntry;

	struct ExplosionProperties {
		ExplosionProperties() {
		}

		ExplosionProperties(const Animation& animation,
			mw::Sound sound = mw::Sound(), float delay = 0.3) :

			animation_(animation),
			sound_(sound),
			delay_(delay) {
		}

		Animation animation_;
		mw::Sound sound_;
		float delay_;
	};

	class Explosion {
	public:
		Explosion();

		void draw(float deltaTime, const GameShader& shader);

		bool toBeRemoved() const;

		void restart(Position position, float blastRadius, const ExplosionProperties& exPr);

	private:
		Position position_;
		float particleSize_;
		float blastRadius_;
		Animation animation_;

		float delay_;
		mw::Sound sound_;
	};

	ExplosionProperties loadExplosion(ZombieEntry& entry);

} // Namespace zombie.

#endif // EXPLOSION_H
