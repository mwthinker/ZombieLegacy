#include "zombieanimation.h"
#include "gamesprite.h"
#include "typedefs.h"
#include "auxiliary.h"

namespace zombie {

	ZombieAnimation::ZombieAnimation(Unit* unit) : idUnit_(unit->getId()) {
		connection_ = unit->addEventHandler(std::bind(&ZombieAnimation::unitEventHandler,this,std::placeholders::_1));

		timeNewFrame_ = 0.0;
		color_ = Color();

		sprites_.push_back(zombie1Sprite);
		sprites_.push_back(zombie2Sprite);
		sprites_.push_back(zombie3Sprite);
		sprites_.push_back(zombie4Sprite);
		sprites_.push_back(zombie5Sprite);
		sprites_.push_back(zombie6Sprite);
		index_ = (int) (random() * sprites_.size());
	}

	ZombieAnimation::~ZombieAnimation() {
		connection_.disconnect();
	}

	bool ZombieAnimation::isRunning() const {
		return Object::getObject(idUnit_) != nullptr;
	}

	void ZombieAnimation::draw(double time) {
		const Object* ob = Object::getObject(idUnit_);

		if (ob != nullptr) {
			const Unit* unit = static_cast<const Unit*>(ob);

			// Time is much larger?
			if (time > timeNewFrame_ + 1) {
				// In order for frames to sync to current time.
				timeNewFrame_ = 0.18 + time;
			}

			if (time > timeNewFrame_) {
				index_ = (1 + index_) % sprites_.size();
				timeNewFrame_ += 0.18;
			}

			Position p = unit->getPosition();

			// Draw body.
			color_.glColor3d();
			glPushMatrix();
			glTranslated(unit->getPosition().x,unit->getPosition().y,0);
			glScaled(unit->radius()*0.9,unit->radius()*0.9,1);
			glRotated(unit->getState().angle_*180/mw::PI,0,0,1);
			double d = sprites_[index_].getTexture()->getWidth();
			glScaled(d/128.0,d/128.0,1);
			sprites_[index_].draw();
			glPopMatrix();
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
