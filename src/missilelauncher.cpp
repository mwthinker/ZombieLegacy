#include "missilelauncher.h"
#include "unit.h"
#include "auxiliary.h"
#include "gameinterface.h"

#include <cassert>

namespace zombie {
	
	MissileLauncher::MissileLauncher(int clipSize,
		float timeBetweenShots, float range) {
		
		clipSize_ = clipSize;
		timeBetweenShots_ = timeBetweenShots;
		range_ = range;
		bulletsInWeapon_ = 1;
		lastShotTime_ = 0.0f;
	}

	void MissileLauncher::pullTrigger(Unit& unit, float time) {
		if (lastShotTime_ + timeBetweenShots_ < time) {
			if (bulletsInWeapon_ > 0) {
				lastShotTime_ = time;
				--bulletsInWeapon_;
				gameInterface_->weaponShot(unit);
				//missile->createBody(unit.getBody()->GetWorld(), unit.getPosition() + (unit.getRadius() + missile->getLength() * 0.5f) * directionVector(unit.getDirection()), unit.getDirection());
				// The missile now belongs to the box2d world.
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
