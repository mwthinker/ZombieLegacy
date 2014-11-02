#ifndef GUN_H
#define GUN_H

#include "weaponinterface.h"
#include "state.h"
#include "box2ddef.h"
#include "gamedataentry.h"
#include "weapon2d.h"

#include <mw/sound.h>

#include <memory>

namespace zombie {

	Weapon2D loadGun(GameInterface* gameInterface, GameDataEntry& entry);

	class Gun : public WeaponInterface {
	public:
		Gun(float damage, float timeBetweenShots, float range, int clipSize,
			mw::Sound shot = mw::Sound(), mw::Sound reload = mw::Sound());

		void pullTrigger(Unit& unit, float time) override;

		void releaseTrigger(Unit& unit, float time) override;

		int getClipSize() const override;

		int getBulletsInWeapon() const override;

		void reload(float time) override;

		float getRange() const override;

		void initEngine(b2World* world, GameInterface* gameInterface);

		WeaponInterfacePtr clone() const override;

	private:
		GameInterface* gameInterface_;
		b2World* world_;

		float damage_;				// The damage made by the weapon.
		float timeBetweenShots_;
		float range_;				// The range of the weapon.

		int clipSize_;				// The number of bullets for a reload.
		int bulletsInWeapon_;		// The current number of bullets in the weapon.
		float lastShotTime_;

		mw::Sound shot_, reload_;
	};

} // Namespace zombie.

#endif // GUN_H
