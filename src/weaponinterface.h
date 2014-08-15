#ifndef WEAPONINTERFACE_H
#define WEAPONINTERFACE_H

#include "auxiliary.h"
#include "state.h"
#include "box2ddef.h"
#include "gameinterface.h"

#include <mw/sound.h>

#include <memory>

namespace zombie {

	class WeaponInterface;
	typedef std::shared_ptr<WeaponInterface> WeaponInterfacePtr;

	// Describes a Weapon and is responsible of shooting.
	class WeaponInterface {
	public:
		virtual void pullTrigger(Unit& unit, float time) {
		}

		virtual void releaseTrigger(Unit& unit, float time) {
		}
		
		virtual void reload(float time) {
		}

		virtual float getRange() const = 0;

		virtual int getClipSize() const = 0;

		virtual int getBulletsInWeapon() const = 0;

		// Should be called by ZombieEngine.
		virtual void initEngine(b2World* world_, GameInterface* gameInterface) {
		}

		virtual WeaponInterfacePtr clone() const = 0;
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
