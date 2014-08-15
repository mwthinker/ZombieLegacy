#ifndef WEAPONEFFECT_H
#define WEAPONEFFECT_H

#include "box2ddef.h"
#include "animation.h"
#include "weapon.h"

#include <vector>
#include <string>

namespace zombie {
	
	class WeaponEffect {
	public:
		virtual void playShotSound() {
		}

		virtual void playReloadSound() {
		}
	private:

	};

} // Namespace zombie.

#endif // WEAPONEFFECT_H
