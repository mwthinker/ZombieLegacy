#ifndef MISSILEPROPERTIES_H
#define MISSILEPROPERTIES_H

#include "animation.h"

#include <mw/sound.h>

#include <string>

namespace zombie {

	struct MissileProperties {
		std::string name_;
		float mass_;
		float width_;
		float length_;
		Animation animation_;
		mw::Sound moveSound_;
	};

} // Namespace zombie.

#endif // MISSILEPROPERTIES_H
