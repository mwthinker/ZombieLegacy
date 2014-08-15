#ifndef WEAPONINTERFACE_H
#define WEAPONINTERFACE_H

#include "state.h"
#include "box2ddef.h"
#include "gameinterface.h"

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
	};

} // Namespace zombie.

#endif // WEAPONINTERFACE_H
