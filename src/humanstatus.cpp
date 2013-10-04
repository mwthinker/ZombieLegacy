#include "humanstatus.h"
#include "gamefont.h"
#include "unit.h"

#include <SDL_opengl.h>

#include <sstream>

namespace zombie {

	HumanStatus::HumanStatus(Unit* unit, PlayerNumber player) : unitId_(unit->getId()) {
		lastTime_ = 0.0;
		name_ = mw::Text("", font15);
		ammo_ = mw::Text("", font15);
		life_ = mw::Text("", font15);
		player_ = player;
	}

	bool HumanStatus::update(double time) {
		draw(time-lastTime_);

		return Object::getObject(unitId_) != nullptr;
	}

	void HumanStatus::draw(double timestep) {
		const Unit* unit = static_cast<const Unit*>(Object::getObject(unitId_));
		if (unit != nullptr) {
			Position p = unit->getPosition();
			glColor3d(1,1,1);

			name_.setText("Human");
			life_.setText("Life");

			std::stringstream stream;
			Weapon w = unit->getWeapon();
			stream << w.getBulletsInWeapon() << " (" << w.clipSize() << ")";
			ammo_.setText(stream.str());
			stream.str("");
			stream <<  " (" << unit->healthPoints() << ")";
			life_.setText(stream.str());

			glPushMatrix();
			glMatrixMode(GL_PROJECTION);
			glPushMatrix();
			glLoadIdentity();
			double proportion = Task::width * 1.0 / Task::height;
			glOrtho(0,proportion*500,0,500,-1,1);
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();

			//glScaled(100,100,1);
			glTranslated(0,75,0);
			name_.draw();

			//glTranslated(0,-font15->getCharacterSize()*1.2,0);
			glTranslated(0,-25,0);

			ammo_.draw();

			glTranslated(0,-25,0);

			life_.draw();

			glMatrixMode(GL_PROJECTION);
			glPopMatrix();
			glMatrixMode(GL_MODELVIEW);
			glPopMatrix();
		}
	}

} // Namespace zombie.
