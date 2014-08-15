#ifndef MISSILELAUNCHER_H
#define MISSILELAUNCHER_H

#include "object.h"
#include "state.h"
#include "weaponinterface.h"

#include <mw/sound.h>

namespace zombie {

	class MissileLauncher : public WeaponInterface {
	public:
		MissileLauncher(float damage, float timeBetweenShots, float range, mw::Sound shot = mw::Sound(), mw::Sound reload = mw::Sound());

		void pullTrigger(Unit& unit, float time) override;

		void releaseTrigger(Unit& unit, float time) override;

		int getClipSize() const override;

		int getBulletsInWeapon() const override;

		void reload(float time) override;

		float getRange() const override;

		void initEngine(b2World* world, GameInterface* gameInterface) override;

		WeaponInterfacePtr clone() const override;

	private:
		GameInterface* gameInterface_;
		b2World* world_;

		float damage_;				// The damage made by the weapon.
		float timeBetweenShots_;
		float range_;				// The range of the weapon.

		int bulletsInWeapon_;		// The current number of bullets in the weapon.
		float lastShotTime_;

		mw::Sound shot_, reload_;
	};

} // Namespace zombie.

#endif // MISSILELAUNCHER_H
