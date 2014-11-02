#include "missilelauncher.h"
#include "unit.h"
#include "auxiliary.h"
#include "gameinterface.h"

#include <cassert>

namespace zombie {
	
	MissileLauncher::MissileLauncher(int clipSize, float timeBetweenShots, float range,
		float damageRadius, float damage, float deathTime, float speed) {
		
		clipSize_ = clipSize;
		timeBetweenShots_ = timeBetweenShots;
		range_ = range;
		bulletsInWeapon_ = 1;
		lastShotTime_ = 0.0f;
		damageRadius_ = damageRadius;
		damage_ = damage;
		deathTime_ = deathTime;
		speed_ = speed;
	}

	void MissileLauncher::pullTrigger(Unit& unit, float time) {
		if (lastShotTime_ + timeBetweenShots_ < time) {
			if (bulletsInWeapon_ > 0) {
				lastShotTime_ = time;
				--bulletsInWeapon_;
				gameInterface_->shot(unit, speed_, deathTime_, damage_, damageRadius_);
			}
		}
	}

	void MissileLauncher::releaseTrigger(Unit& unit, float time) {
	}

	int MissileLauncher::getClipSize() const {
		return clipSize_;
	}

	int MissileLauncher::getBulletsInWeapon() const {
		return bulletsInWeapon_;
	}

	float MissileLauncher::getRange() const {
		return range_;
	}

	// Tries to reload the weapon. If it reloads return true, else false.
	void MissileLauncher::reload(float time) {
		if (bulletsInWeapon_ < clipSize_) {
			reload();
			bulletsInWeapon_ += 1;
		}
	}

	void MissileLauncher::initEngine(b2World* world, GameInterface* gameInterface) {
		gameInterface_ = gameInterface;
		world_ = world;
	}

} // Namespace zombie.
