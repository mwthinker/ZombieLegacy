#include "missilelauncher2d.h"
#include "gameinterface.h"
#include "laser.h"

namespace zombie {

	Weapon2D loadMissileLauncher2D(GameInterface* gameInterface, const WeaponProperties& wp) {
		std::shared_ptr<Laser> laser;
		if (false) {
			//laser = loadLaser(entry.getParent().getParent().getChildEntry("laserSight"));
		}
		auto missileLauncher = std::make_shared<MissileLauncher2D>(wp.clipSize_, wp.timeBetweenShots_,
			wp.range_, wp.damageRadius_, wp.damage_, wp.deathTime_, wp.speed_, wp.force_, wp.shootSound_, wp.reloadSound_);

		if (false) {
			return Weapon2D(missileLauncher, wp.symbolImage_, wp.moveAnimation_, wp.size_, wp.moveImageGrip_, *laser);
		} else {
			return Weapon2D(missileLauncher, wp.symbolImage_, wp.moveAnimation_, wp.size_, wp.moveImageGrip_);
		}
	}

} // Namespace zombie.
