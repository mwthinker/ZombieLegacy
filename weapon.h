#ifndef WEAPON_H
#define WEAPON_H

#include <mw/packet.h>

namespace zombie {

// Describes a Weapon and is responsible of shooting.
class Weapon {
public:
	Weapon(double damage, double timeBetweenShots, double range, int clipSize) {
		damage_ = damage;
		timeBetweenShots_ = timeBetweenShots;
		range_ = range;
		clipSize_ = clipSize;
		bulletsInWeapon_ = clipSize;
		lastShotTime_ = 0.0;
		shotSound_ = range_;
	}

	Weapon() {
		damage_ = 35;
		timeBetweenShots_ = 0.1;
		range_ = 30;
		clipSize_ = 12;
		bulletsInWeapon_ = 12;
		lastShotTime_ = 0.0;
		shotSound_ = range_;
	}

	friend mw::Packet& operator<<(mw::Packet& packet, const Weapon& weapon);
	friend mw::Packet& operator>>(mw::Packet& packet, Weapon& weapon);

	// Called when user pulls the trigger.
	bool shoot(double time) {
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
	double damage() const {
		return damage_;
	}	

	//
	double range() const {
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
	
	double damage_;  // The damage made by the weapon.
	double timeBetweenShots_;
	double range_;        // The range of the weapon.
	
	int clipSize_;        // The number of bullets for a reload.
	int bulletsInWeapon_; // The current number of bullets in the weapon.
	double shotSound_;
	
	double timeElapsed_;  // Used as keeping track of the time.
	double lastShotTime_;	
};

} // namespace zombie.

#endif // WEAPON_H
