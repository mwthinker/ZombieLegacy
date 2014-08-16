#ifndef MISSILELAUNCHER_H
#define MISSILELAUNCHER_H

#include "object.h"
#include "state.h"
#include "missile.h"
#include "weaponinterface.h"

namespace zombie {

	class MissileLauncher : public WeaponInterface {
	public:
		MissileLauncher(int clipSize, float timeBetweenShots,
			float range);

		void pullTrigger(Unit& unit, float time) override final;

		void releaseTrigger(Unit& unit, float time) override final;

		int getClipSize() const override final;

		int getBulletsInWeapon() const override final;

		void reload(float time) override final;

		float getRange() const override final;

		void initEngine(b2World* world, GameInterface* gameInterface) override final;

	protected:
		// Is called when a new missile is launched. The missile returned is
		// used as the project tile. The ownership takes over by the box2d world.
		virtual Missile* shot() = 0;
		
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
	};

} // Namespace zombie.

#endif // MISSILELAUNCHER_H
