#ifndef MISSILELAUNCHER2D_H
#define MISSILELAUNCHER2D_H

#include "missilelauncher.h"
#include "missile2d.h"

#include <mw/sound.h>

#include <memory>

namespace zombie {

	class MissileLauncher2D : public MissileLauncher {
	public:
		MissileLauncher2D(const Missile2D& missile, int clipSize, float timeBetweenShots,
			float range, mw::Sound shot = mw::Sound(), mw::Sound reload = mw::Sound())
			: MissileLauncher(clipSize, timeBetweenShots, range), missile_(missile) {

		}

		WeaponInterfacePtr clone() const override {
			return std::make_shared<MissileLauncher2D>(*this);
		}

	protected:
		Missile2D* shot() override {
			mw::Sound sound = shot_;
			shot_.play();
			return new Missile2D(missile_);
		}
		
		void reload() override {
			mw::Sound sound = shot_;
			reload_.play();
		}

	private:
		mw::Sound shot_, reload_;
		Missile2D missile_;
	};

} // Namespace zombie.

#endif // MISSILELAUNCHER2D_H
