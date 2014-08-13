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
	class WeaponInterface : public Weapon {
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

} // Namespace zombie.

#endif // WEAPONINTERFACE_H
