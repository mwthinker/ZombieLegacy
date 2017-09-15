#ifndef UNITPROPERTIES_H
#define UNITPROPERTIES_H

#include "box2ddef.h"
#include "auxiliary.h"
#include "weapon.h"
#include "animation.h"
#include "weaponproperties.h"

#include <mw/opengl.h>
#include <mw/color.h>
#include <mw/sound.h>
#include <mw/sprite.h>

#include <vector>

namespace zombie {

	struct UnitProperties {
		float mass_;
		float radius_;
		float life_;
		float walkingSpeed_;
		float runningSpeed_;
		float stamina_;
		Position grip_;

		Animation moveAnimation_;
		Animation dieAnimation_;
		Animation injuredAnimation_;
		WeaponProperties weaponProperties_;
		mw::Sound hitSound_;
		mw::Sound dieSound_;
	};

} // Namespace zombie.

#endif // UNITPROPERTIES_H
