#ifndef MISSILE2D_H
#define MISSILE2D_H

#include "missile.h"
#include "animation.h"
#include "gamedataentry.h"
#include "gameinterface.h"

#include <mw/opengl.h>

namespace zombie {	

	struct MissileProperties {
		MissileProperties() {
		}

		MissileProperties(float width, float length, float mass, float speed,
			float explosionTime, float explosionRadius, float damage) {
			width_ = width;
			length_ = length;
			mass_ = mass;
			speed_ = speed;
			explosionTime_ = explosionTime;
			explosionRadius_ = explosionRadius;
			damage_ = damage;
		}

		Animation animation_;
		float length_, width_;
		float speed_;
		float damage_;
		float mass_;
		float explosionRadius_;
		float explosionTime_;
	};

	MissileProperties loadMissileProperties(GameDataEntry& entry);

	class Missile2D : public Missile {
	public:
		inline Missile2D() {
		}

		Missile2D(const MissileProperties& properties, GameInterface* gameInterface);

		void draw(float accumulator, float timeStep, const GameShader& gameShader);

	private:
		Animation animation_;
		State previousState_;
	};

} // Namespace zombie.

#endif // MISSILE_H
