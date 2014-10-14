#ifndef MISSILE2D_H
#define MISSILE2D_H

#include "missile.h"
#include "animation.h"
#include "gamedataentry.h"
#include "gameinterface.h"

#include <mw/opengl.h>

namespace zombie {	

	class Missile2D : public Missile {
	public:
		Missile2D(const Animation& animation, GameInterface& gameInterface, float width, float length,
			float mass, float speed, float explodeTime, float damage, float explosionRadius);

		void draw(float accumulator, float timeStep, const GameShader& gameShader);

	private:
		Animation animation_;
		State previousState_;
	};

	Missile2D loadMissile2D(GameInterface* gameInterface, GameDataEntry& entry, float damage, float range);

} // Namespace zombie.

#endif // MISSILE_H
