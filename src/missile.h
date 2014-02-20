#ifndef MISSILE_H
#define MISSILE_H

#include "object.h"
#include "state.h"

#include <SDL_opengl.h>

namespace zombie {

	class Missile : public Object {
	public:
		Missile(float range, float damage, float explosionRadius) {
			range_ = range;
			damage_ = damage;
			explosionRadius_ = explosionRadius;
		}

		void createBody(b2World* world, State state) {

		}

		void draw(float time) override {
		}

		bool toBeRemoved() const {
			return true;
		}

	private:
		float range_;
		float damage_;
		float explosionRadius_;
	};

} // Namespace zombie.

#endif // MISSILE_H
