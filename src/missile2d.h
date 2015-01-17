#ifndef MISSILE2D_H
#define MISSILE2D_H

#include "missile.h"
#include "animation.h"
#include "zombieentry.h"
#include "gameinterface.h"

#include <mw/sound.h>
#include <mw/opengl.h>

namespace zombie {

	class Missile2D : public Missile {
	public:
		inline Missile2D() {
		}

		Missile2D(GameInterface* gameInterface, float width, float length, float mass,
			const Animation& animation, const mw::Sound& moveSound);

		void draw(float accumulator, float timeStep, const GameShader& gameShader);

	private:
		Animation animation_;
		State previousState_;
	};

	Missile2D loadMissile2D(GameInterface* gameInterface, ZombieEntry entry);

} // Namespace zombie.

#endif // MISSILE_H
