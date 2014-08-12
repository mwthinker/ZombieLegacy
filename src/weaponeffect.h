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
		virtual void update(double timestep) = 0;
	private:

	};

	class HandgunEffect : WeaponEffect{
		void update(double timestep) {
			// Do something!
		}

	};

	class BazookaEffect : WeaponEffect{
		void update(double timestep) {
			// Do something!
		}

	};

} // Namespace zombie.

#endif // WEAPONEFFECT_H
