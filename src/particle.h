#ifndef PARTICLE_H
#define PARTICLE_H

#include "box2ddef.h"

#include <mw/color.h>

namespace zombie {

	class Particle {
	public:
		Position pos_;
		Position vel_;
		bool alive;
		float lifeTime_;
		float particleSize_;
		float angle_;
	};

} // Namespace zombie.

#endif // PARTICLE_H
