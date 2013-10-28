#include "zombieanimation.h"
#include "gamesprite.h"
#include "typedefs.h"
#include "auxiliary.h"

namespace zombie {

	ZombieAnimation::ZombieAnimation(Unit* unit, const Animation& animation) : idUnit_(unit->getId()), animation_(animation) {
		connection_ = unit->addEventHandler(std::bind(&ZombieAnimation::unitEventHandler,this,std::placeholders::_1));
	}

	ZombieAnimation::~ZombieAnimation() {
		connection_.disconnect();
	}

	bool ZombieAnimation::update(double time) {
		const Object* ob = Object::getObject(idUnit_);

		if (ob != nullptr) {
			const Unit* unit = static_cast<const Unit*>(ob);
			Position p = unit->getPosition();

			// Draw body.			
			glPushMatrix();
			glTranslated(unit->getPosition().x,unit->getPosition().y,0);
			glScaled(unit->getRadius()*0.9,unit->getRadius()*0.9,1);
			glRotated(unit->getState().angle_*180/PI,0,0,1);
			animation_.draw(time);
			glPopMatrix();
			return true;
		} else {
			return false;
		}
	}

	void ZombieAnimation::unitEventHandler(Unit::UnitEvent unitEvent) {
		switch (unitEvent) {
		case Unit::UnitEvent::SHOOT:
			//attack_.play();
			break;
		case Unit::UnitEvent::RELOADING:
			break;
		case Unit::UnitEvent::DIE:
			break;
		}
	}

} // Namespace zombie.
