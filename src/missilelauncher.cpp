#include "missilelauncher.h"

namespace zombie {

	MissileLauncher::MissileLauncher(float damage, float timeBetweenShots, float range, mw::Sound shot, mw::Sound reload) : shot_(shot), reload_(reload) {
		damage_ = damage;
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
				mw::Sound sound = shot_;
				sound.play();
			}
		}
	}

	void MissileLauncher::releaseTrigger(Unit& unit, float time) {
	}

	int MissileLauncher::getClipSize() const {
		return 1;
	}

	int MissileLauncher::getBulletsInWeapon() const {
		return bulletsInWeapon_;
	}

	float MissileLauncher::getRange() const {
		return range_;
	}

	// Tries to reload the weapon. If it reloads return true, else false.
	void MissileLauncher::reload(float time) {
		if (bulletsInWeapon_ == 0) {
			mw::Sound sound = reload_;
			sound.play();
			bulletsInWeapon_ = 1;
		}
	}

	void MissileLauncher::initEngine(b2World* world, GameInterface* gameInterface) {
		gameInterface_ = gameInterface;
		world_ = world;
	}

	WeaponInterfacePtr MissileLauncher::clone() const {
		return std::make_shared<MissileLauncher>(*this);
	}

} // Namespace zombie.
