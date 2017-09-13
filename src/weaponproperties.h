#ifndef WEAPONPROPERTIES_H
#define WEAPONPROPERTIES_H

#include "box2ddef.h"
#include "auxiliary.h"
#include "weapon.h"
#include "animation.h"
#include "missileproperties.h"

#include <mw/opengl.h>
#include <mw/color.h>
#include <mw/sound.h>
#include <mw/sprite.h>

#include <vector>
#include <string>

namespace zombie {

	struct WeaponProperties {
		enum Type {BULLET, MISSILE};

		std::string name_;
		Type type_;
		mw::Sprite symbolImage_;
		float timeBetweenShots_;
		int clipSize_;
		mw::Sound shootSound_;
		mw::Sound reloadSound_;
		Animation moveAnimation_;
		float size_;
		Position moveImageGrip_;
		MissileProperties missileProperties_;

		float damageRadius_;
		float damage_;
		float range_;
		float deathTime_;
		float speed_;
		float force_;

		bool laserSight_;
	};

} // Namespace zombie.

#endif // WEAPONPROPERTIES_H
