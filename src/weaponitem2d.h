#ifndef WEAPONITEM2d_H
#define WEAPONITEM2d_H

#include "weaponitem.h"
#include "weapon.h"

namespace zombie {

	class WeaponItem2D : public WeaponItem {
	public:
		WeaponItem2D(float x, float y, const WeaponPtr& weapon) : WeaponItem(x, y, weapon) {
		}

		void draw(float accumulator, float timeStep) override {
			/*
			const WeaponObject* wOb = static_cast<const WeaponObject*>(ob);
			Position p = wOb->getPosition();
			glColor3d(0,0,1);
			drawCircle(p.x,p.y,wOb->getRadius(),6,true);
			return true;
			*/
		}

	};

} // Namespace zombie.

#endif // WEAPONITEM2d_H
