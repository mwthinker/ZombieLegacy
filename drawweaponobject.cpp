#include "drawweaponobject.h"
#include "weaponobject.h"
#include "task.h"

#include <SDL_opengl.h>

using namespace zombie;

DrawWeaponObject::DrawWeaponObject(WeaponObject* wOb) {
	inMemory_ = wOb->getInMemory();
	wOb_ = wOb;
}

void DrawWeaponObject::draw(double time) {
	if (inMemory_.isValid()) {
		Position p = wOb_->getPosition();
		glColor3d(0,0,1);
		drawCircle(p.x,p.y,wOb_->getRadius(),6,true);
	}
}

bool DrawWeaponObject::isRunning() const {
	return inMemory_.isValid();
}
