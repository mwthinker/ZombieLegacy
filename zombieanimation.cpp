#include "zombieanimation.h"
#include "gamesprite.h"
#include "color.h"
#include "auxiliary.h"

namespace zombie {

	ZombieAnimation::ZombieAnimation(Unit* unit) {
		unit_ = unit;
		connection_ = unit->addEventHandler(std::bind(&ZombieAnimation::unitEventHandler,this,std::placeholders::_1));

		timeNewFrame_ = 0.0;
		color_ = Color();

		sprites_.push_back(zombie1);
		sprites_.push_back(zombie2);
		sprites_.push_back(zombie3);
		sprites_.push_back(zombie4);
		sprites_.push_back(zombie5);
		sprites_.push_back(zombie6);
		index_ = (int) (random() * sprites_.size());
	}

	ZombieAnimation::~ZombieAnimation() {
		connection_.disconnect();
	}

	void ZombieAnimation::drawSecond(double time) {
		draw(0.0);
		//if (walk_) 

		// Time is much larger?
		if (time > timeNewFrame_ + 1) {
			// In order for frames to sync to current time.
			timeNewFrame_ = 0.18 + time;
		}

		if (time > timeNewFrame_) {
			index_ = (1 + index_) % sprites_.size();
			timeNewFrame_ += 0.18;
		}
	}

	bool ZombieAnimation::isRunning() const {
		return !unit_->isDead();
	}

	void ZombieAnimation::draw(double timestep) {
		Position p = unit_->getPosition();

		// Draw body
		color_.glColor3d();
		glPushMatrix();
		glTranslated(unit_->getPosition().x,unit_->getPosition().y,0);
		glScaled(unit_->radius()*0.9,unit_->radius()*0.9,1);
		glRotated(unit_->getState().angle_*180/mw::PI,0,0,1);
		glScaled(1069.0/128.0,1069.0/128.0,1);
		sprites_[index_].draw();
		glPopMatrix();
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
