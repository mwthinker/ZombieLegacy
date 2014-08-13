#ifndef WEAPONINTERFACE_H
#define WEAPONINTERFACE_H

#include "weapon.h"
#include "animation.h"
#include "auxiliary.h"

#include <mw/sprite.h>
#include <mw/sound.h>

#include <memory>

namespace zombie {

	// Describes a Weapon and is responsible of shooting.
	class WeaponInterface {
	public:
		WeaponInterface() {
		}

		virtual void shoot() = 0;
		virtual void reload() = 0;

		// For possible use later on. Movement might affect accuracy. Entering a car might abort reloading ... etc.
		void forward() {};
		void backward() {};
		void turnLeft() {};
		void turnRight() {};
		void run() {};
		void action() {};
	private:
	};

	class Handgun : public WeaponInterface {
	public:	
		void shoot() {
			// UNIT WANT TO SHOOT, READY?
		}

		void reload() {
		
		}
	private:
		bool reload_;				// Is true when the weapon is reloading.
		float damage_;				// The damage made by the weapon.
		float timeBetweenShots_;
		float range_;				// The range of the weapon.
		int clipSize_;				// The number of bullets for a reload.
		int bulletsInWeapon_;		// The current number of bullets in the weapon.
		float shotSound_;
		float lastShotTime_;
	};

	class Bazooka : public WeaponInterface {
	public:
		void shoot() {
			// UNIT WANT TO SHOOT, READY?
		}

		void reload() {

		}
	private:
		bool reload_;				// Is true when the weapon is reloading.
		float timeBetweenShots_;
		float range_;				// The range of the weapon.
		int clipSize_;				// The number of bullets for a reload.
		int bulletsInWeapon_;		// The current number of bullets in the weapon.
		float shotSound_;
		float lastShotTime_;
	};

} // Namespace zombie.

#endif // WEAPONINTERFACE_H
