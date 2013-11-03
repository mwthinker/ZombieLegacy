#include "humananimation.h"
#include "gamesprite.h"
#include "gamesound.h"
#include "typedefs.h"
#include "auxiliary.h"
#include "unit.h"

namespace zombie {

	HumanAnimation::HumanAnimation(Unit* unit, const Animation& animation) : animation_(animation) {
		connection_ = unit->addEventHandler(std::bind(&HumanAnimation::unitEventHandler,this,std::placeholders::_1));
		humanId_ = unit->getId();
	}

	HumanAnimation::~HumanAnimation() {
		connection_.disconnect();
	}

	// private
	bool HumanAnimation::update(double time) {
		const Object* ob = Object::getObject(humanId_);

		// Object alive and active?
		if (ob != nullptr && ob->getBody()->IsActive()) {
			const Unit* unit = static_cast<const Unit*>(ob);
			
			Position p = unit->getPosition();

			// Draw body
			glPushMatrix();
			glTranslated(p.x,p.y,0);
			glScaled(unit->getRadius()*0.9,unit->getRadius()*0.9,1);
			glRotated(unit->getState().angle_*180/PI,0,0,1);
			animation_.draw(time);
			glPopMatrix();
			return true;
		}

		return ob != nullptr;
	}

	void HumanAnimation::unitEventHandler(Unit::UnitEvent unitEvent) {
		switch (unitEvent) {
		case Unit::UnitEvent::SHOOT:
			{
				// In order to be able to play even if the sound is not finnished!
				mw::Sound tmp = shot;
				tmp.play();
			}
			break;
		case Unit::UnitEvent::RELOADING:
			{
				// In order to be able to play even if the sound is not finnished!
				mw::Sound tmp = reload;
				tmp.play();
			}
			break;
		case Unit::UnitEvent::DIE:
			break;
		case Unit::UnitEvent::STANDSTILL:
			animation_.restart();
			break;
		case Unit::UnitEvent::WALK:
			// Fall through!
		case Unit::UnitEvent::RUN:
			break;
		}
	}

} // Namespace zombie.
