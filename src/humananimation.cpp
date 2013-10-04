#include "humananimation.h"
#include "gamesprite.h"
#include "gamesound.h"
#include "typedefs.h"
#include "auxiliary.h"
#include "unit.h"

namespace zombie {

	HumanAnimation::HumanAnimation(Unit* unit) {
		connection_ = unit->addEventHandler(std::bind(&HumanAnimation::unitEventHandler,this,std::placeholders::_1));
		humanId_ = unit->getId();

		timeNewFrame_ = 0.0;
		index_ = 0;
		lastTime_ = 0.0;
		color_ = Color();

		sprites_.push_back(human1Sprite);
		sprites_.push_back(human2Sprite);
		sprites_.push_back(human1Sprite);
		sprites_.push_back(human3Sprite);
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
			lastTime_ = time;

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

			// Draw body
			color_.glColor3d();
			glPushMatrix();
			glTranslated(p.x,p.y,0);
			glScaled(unit->getRadius()*0.9,unit->getRadius()*0.9,1);
			glRotated(unit->getState().angle_*180/mw::PI,0,0,1);
			mw::TexturePtr texture = sprites_[index_].getTexture();
			glScaled(texture->getWidth()/128.0,texture->getHeight()/128.0,1);
			sprites_[index_].draw();
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
