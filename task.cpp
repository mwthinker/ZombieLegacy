#include "task.h"

#include "unit.h"
#include "building.h"
#include "gamesound.h"
#include "gamefont.h"

#include <mw/text.h>
#include <SDL_opengl.h>

#include <memory>
#include <sstream>
#include <vector>
#include <queue>
#include <cmath>

namespace zombie {

	int Task::width = 0;
	int Task::height = 0;

	Task* Task::pull() {
		if (tasks_.empty()) {
			return nullptr;
		}

		Task* task = tasks_.front();
		tasks_.pop();
		return task;
	}	

	void Task::push(Task* task) {
		tasks_.push(task);
	}
	
	void drawCircle(double cx, double cy, double r, int num_segments, bool filled) {
		double theta = 2 * 3.1415926 / double(num_segments);
		double c = std::cos(theta);
		double s = std::sin(theta);
		double t;

		double x = r ;// Start at angle = 0
		double y = 0;

		if (filled) {
			glBegin(GL_TRIANGLE_FAN);
			glVertex2d(cx,cy);
		} else {
			glBegin(GL_LINE_LOOP);
		}
		for(int i = 0; i < num_segments+1; i++) {
			glVertex2d(x + cx, y + cy);
			//apply the rotation matrix
			t = x;
			x = c * x - s * y;
			y = s * t + c * y;
		}
		glEnd();
	}	

	HumanStatus::HumanStatus(const UnitPtr& unit, Player player) {
		unit_ = unit;
		lastTime_ = 0.0;
		name_ = mw::Text("", font15);
		ammo_ = mw::Text("", font15);
		life_ = mw::Text("", font15);
		player_ = player;
	}

	void HumanStatus::excecute(double time) {
		draw(time-lastTime_);
	}

	bool HumanStatus::isRunning() const {
		return !unit_->isDead();
	}

	void HumanStatus::draw(double timestep) {
		Position p = unit_->getPosition();
		glColor3d(1,1,1);

		name_.setText("Human");
		life_.setText("Life");

		std::stringstream stream;
		Weapon w = unit_->getWeapon();
		stream << w.getBulletsInWeapon() << " (" << w.clipSize() << ")";
		ammo_.setText(stream.str());
		stream.str("");
		stream <<  " (" << unit_->healthPoints() << ")";
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
	
} // Namespace zombie.
