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

	/*
	DrawBuildning::DrawBuildning(const BuildingPtr& building) {
		buildning_ = building;
	}

	void DrawBuildning::excecute(double time) {
		draw();
	}

	bool DrawBuildning::isRunning() const {
		return true;
	}

	//private
	void DrawBuildning::draw() {
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);		
		glColor3d(0.1,0.9,0.4);
		glBegin(GL_LINE_LOOP);
		const auto& corners = buildning_->getCorners();
		for (const Position& p : corners) {
			glVertex2d(p.x_,p.y_);
		}		
		glEnd();		
		glDisable(GL_BLEND);
	}	

	Buildning3DTask::Buildning3DTask(const BuildingPtr& building) {
		buildning_ = building;
	}

	void Buildning3DTask::excecute(double time) {
		draw();
	}

	bool Buildning3DTask::isRunning() const {
		return true;
	}

	//private
	void Buildning3DTask::draw() {
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);		
		
		// Walls.
		glColor3d(0.1,0.9,0.4);
		glBegin(GL_QUAD_STRIP);
		const auto& corners = buildning_->getCorners();
		int size = corners.size();
		for (int i = 0; i < size; ++i) {
			glVertex3d(corners[i].x_,corners[i].y_,0);
			glVertex3d(corners[i].x_,corners[i].y_,1);
		}
		glEnd();

		// Roof.
		glColor3d(0.6,0.2,0.4);
		Position centre = buildning_->getPosition();
		glBegin(GL_TRIANGLE_FAN);
		glVertex3d(centre.x_,centre.y_,1);
		for (int i = 0; i < size; ++i) {
			glVertex3d(corners[i].x_,corners[i].y_,0);
		}
		glEnd();
		glDisable(GL_BLEND);
	}
	*/

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
		glOrtho(0,Task::width,0,Task::height,-1,1);		
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
		
		// Draw small view sphere
		//drawCircle(p[0],p[1],unit_->smallViewDistance(),20,false);
	}

	/*
	Buildning3DTask::Buildning3DTask(const BuildingPtr& building) {
		buildning_ = building;
	}

	void Buildning3DTask::drawFirst(double time) {
		draw();
	}

	bool Buildning3DTask::isRunning() const {
		return true;
	}

	//private
	void Buildning3DTask::draw() {
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);		
		
		// Walls.
		glColor3d(0.1,0.9,0.4);
		glBegin(GL_QUAD_STRIP);
		const auto& corners = buildning_->getCorners();
		int size = corners.size();
		for (int i = 0; i < size; ++i) {
			glVertex3d(corners[i].x_,corners[i].y_,0);
			glVertex3d(corners[i].x_,corners[i].y_,1);
		}
		glEnd();

		// Roof.
		glColor3d(0.6,0.2,0.4);
		Position centre = buildning_->getPosition();
		glBegin(GL_TRIANGLE_FAN);
		glVertex3d(centre.x_,centre.y_,1);
		for (int i = 0; i < size; ++i) {
			glVertex3d(corners[i].x_,corners[i].y_,0);
		}
		glEnd();
		glDisable(GL_BLEND);
	}
	*/
} // Namespace zombie.