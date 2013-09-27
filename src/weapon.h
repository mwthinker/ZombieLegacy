#ifndef WEAPON_H
#define WEAPON_H

namespace zombie {

	// Describes a Weapon and is responsible of shooting.
	class Weapon {
	public:
		Weapon() {
			damage_ = 35.f;
			timeBetweenShots_ = 0.1f;
			range_ = 30.f;
			clipSize_ = 12;
			bulletsInWeapon_ = 12;
			lastShotTime_ = 0.0f;
			shotSound_ = range_;
		}

		Weapon(float damage, float timeBetweenShots, float range, int clipSize) {
			damage_ = damage;
			timeBetweenShots_ = timeBetweenShots;
			range_ = range;
			clipSize_ = clipSize;
			bulletsInWeapon_ = clipSize;
			lastShotTime_ = 0.0f;
			shotSound_ = range_;
		}

		// Called when user pulls the trigger.
		bool shoot(float time) {
			if (lastShotTime_ + timeBetweenShots_ < time) {
				if (bulletsInWeapon_ > 0) {
					lastShotTime_ = time;
					--bulletsInWeapon_;
					return true;
				}
			}
			return false;
		}

		// Tries to reload the weapon. If it reloads return true, else false.
		bool reload() {
			bulletsInWeapon_ = clipSize_;
			return true;
		}

		// Returns the point damage.
		float damage() const {
			return damage_;
		}	

		//
		float range() const {
			return range_;
		}

		int clipSize() const {
			return clipSize_;
		}

		int getBulletsInWeapon() const {
			return bulletsInWeapon_;
		}

		double getShotSound() const {
			return shotSound_;
		}
	private:
		bool reload_;         // Is true when the weapon is reloading.

		float damage_;  // The damage made by the weapon.
		float timeBetweenShots_;
		float range_;        // The range of the weapon.

		int clipSize_;        // The number of bullets for a reload.
		int bulletsInWeapon_; // The current number of bullets in the weapon.
		float shotSound_;

		float timeElapsed_;  // Used as keeping track of the time.
		float lastShotTime_;	
	};

} // Namespace zombie.

#endif // WEAPON_H
