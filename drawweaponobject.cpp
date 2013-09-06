#include "drawweaponobject.h"
#include "weaponobject.h"
#include "task.h"

#include <SDL_opengl.h>

using namespace zombie;

DrawWeaponObject::DrawWeaponObject(WeaponObject* weapon) : idWeaponObject_(weapon->getId()) {
}

void DrawWeaponObject::draw(double time) {
	const Object* ob = Object::getObject(idWeaponObject_);
	if (ob != nullptr) {
		const WeaponObject* wOb = static_cast<const WeaponObject*>(ob);
		Position p = wOb->getPosition();
		glColor3d(0,0,1);
		drawCircle(p.x,p.y,wOb->getRadius(),6,true);
	}
}

bool DrawWeaponObject::isRunning() const {
	return Object::getObject(idWeaponObject_) != nullptr;
}
