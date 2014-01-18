#ifndef WEAPONITEM2d_H
#define WEAPONITEM2d_H

#include "weaponitem.h"
#include "weapon.h"

namespace zombie {

	class WeaponItem2D : public WeaponItem {
	public:
		WeaponItem2D(b2World* world, float x, float y, const Weapon& weapon) : WeaponItem(world, x, y, weapon) {
		}
	};

	void draw(float time) {
		/*
		const WeaponObject* wOb = static_cast<const WeaponObject*>(ob);
		Position p = wOb->getPosition();
		glColor3d(0,0,1);
		drawCircle(p.x,p.y,wOb->getRadius(),6,true);
		return true;
		*/
	}

} // Namespace zombie.

#endif // WEAPONITEM2d_H
