#ifndef MISSILELAUNCHER_H
#define MISSILELAUNCHER_H

#include "object.h"
#include "state.h"
#include "missile.h"
#include "weaponinterface.h"
#include "missile.h"

namespace zombie {

	class MissileLauncher : public WeaponInterface {
	public:
		MissileLauncher(int clipSize, float timeBetweenShots, float range,
			float damageRadius, float damage, float deathTime, float speed, float force);

		void pullTrigger(Unit& unit, float time) override final;

		void releaseTrigger(Unit& unit, float time) override final;

		int getClipSize() const override final;

		int getBulletsInWeapon() const override final;

		void reload(float time) override final;

		float getRange() const override final;

		void initEngine(b2World* world, GameInterface* gameInterface) override final;

	protected:
		// Is called when reloading.
		virtual void reload() {
		}

	private:
		GameInterface* gameInterface_;
		b2World* world_;

		int clipSize_;
		float timeBetweenShots_;
		float range_;				// The range of the weapon.

		int bulletsInWeapon_;		// The current number of bullets in the weapon.
		float lastShotTime_;

		float damageRadius_;
		float damage_;
		float deathTime_;
		float speed_;
		float force_;
	};

} // Namespace zombie.

#endif // MISSILELAUNCHER_H
