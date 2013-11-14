#include "drawweaponobject.h"
#include "weaponobject.h"
#include "task.h"

#include <SDL_opengl.h>

namespace zombie {

	DrawWeaponObject::DrawWeaponObject(WeaponObject* weapon) : idWeaponObject_(weapon->getId()) {
	}

	bool DrawWeaponObject::update(float time) {
		/*
		const WeaponObject* wOb = static_cast<const WeaponObject*>(ob);
		Position p = wOb->getPosition();
		glColor3d(0,0,1);
		drawCircle(p.x,p.y,wOb->getRadius(),6,true);
		return true;
		*/
		return true;
	}

} // Namespace zombie.
