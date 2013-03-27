#include "humananimation.h"
#include "gamesprite.h"
#include "gamesound.h"
#include "color.h"
#include "auxiliary.h"

namespace zombie {

	HumanAnimation::HumanAnimation(Unit* unit) {
		unit_ = unit;
		connection_ = unit->addEventHandler(std::bind(&HumanAnimation::unitEventHandler,this,std::placeholders::_1));

		timeNewFrame_ = 0.0;
		index_ = 0;
		lastTime_ = 0.0;
		color_ = Color();

		sprites_.push_back(human1);
		sprites_.push_back(human2);
		sprites_.push_back(human1);
		sprites_.push_back(human3);
	}

	HumanAnimation::~HumanAnimation() {
		connection_.disconnect();
	}

	void HumanAnimation::drawSecond(double time) {
		lastTime_ = time;
		draw(0.0);

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

	bool HumanAnimation::isRunning() const {
		return !unit_->isDead();
	}

	// private
	void HumanAnimation::draw(double timestep) {
		Position p = unit_->getPosition();

		// Draw body
		color_.glColor3d();
		glPushMatrix();
		glTranslated(p.x,p.y,0);
		glScaled(unit_->radius()*0.9,unit_->radius()*0.9,1);
		glRotated(unit_->getState().angle_*180/mw::PI,0,0,1);
		mw::TexturePtr texture = sprites_[index_].getTexture();
		glScaled(texture->getWidth()/128.0,texture->getHeight()/128.0,1);
		sprites_[index_].draw();
		glPopMatrix();
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
			index_ = 0;
			timeNewFrame_ = lastTime_ + 0.18;
			break;
		case Unit::UnitEvent::WALK:
			// Fall through!
		case Unit::UnitEvent::RUN:
			break;
		}
	}

} // Namespace zombie.
