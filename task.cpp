#include "task.h"

#include <vector>
#include <queue>
#include <SDL_opengl.h>

#include "unit.h"
#include "building.h"

#include <memory>

namespace zombie {

	typedef std::shared_ptr<Unit> UnitPtr;

	Task::Task(int drawOrder) {
		drawOrder_ = drawOrder;
	}

	int Task::getDrawOrder() const {
		return drawOrder_;
	}

	Task* Task::pull() {
		if (tasks_.empty()) {
			return nullptr;
		}

		Task* task = tasks_.front();
		tasks_.pop();
		return task;
	}
	// protected
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


	DrawBuildning::DrawBuildning(const BuildingPtr& building) : Task (1) {
		buildning_ = building;
	}

	void DrawBuildning::excecute(double time) {
		draw();
	}

	bool DrawBuildning::isRunning() const {
		return !buildning_->isDead();
	}

	//private
	void DrawBuildning::draw() {
		glColor3d(1,1,1);
		//glPolygonMode(GL_FRONT, GL_FILL);
		glBegin(GL_LINES);

		const auto& corners = buildning_->getCorners();
		int size = corners.size();
		for (int i = 0; i < size + 1; ++i) {
			Position p1 = corners[i % size];			
			Position p2 = corners[(i+1) % size];
			glVertex2d(p1[0],p1[1]);
			glVertex2d(p2[0],p2[1]);
		}		
		glEnd();		
	}

	HumanAnimation::HumanAnimation(const UnitPtr& unit) : Task (1) {
		unit_ = unit;
	}

	void HumanAnimation::excecute(double time) {
		draw(0.0);
	}

	bool HumanAnimation::isRunning() const {
		return !unit_->isDead();
	}

	// private
	void HumanAnimation::draw(double timestep) {
		Position p = unit_->getPosition();
		glColor3d(1,1,1);
		// Draw body		
		drawCircle(p[0],p[1],unit_->radius(),20,false);

		glColor3d(1,1,1);
				
		// Draw view sphere
		//drawCircle(p[0],p[1],unit_->viewDistance(),20,false);
		glBegin(GL_LINES);
		//glVertex2d(p[0],p[1]);
		//glVertex2d(p[0]+std::cos(unit_->moveDirection() - 0.5*unit_->viewAngle())*unit_->viewDistance(),p[1]+std::sin(unit_->moveDirection() - 0.5*unit_->viewAngle())*unit_->viewDistance());
		//glVertex2d(p[0],p[1]);
		//glVertex2d(p[0]+std::cos(unit_->moveDirection() + 0.5*unit_->viewAngle())*unit_->viewDistance(),p[1]+std::sin(unit_->moveDirection() + 0.5*unit_->viewAngle())*unit_->viewDistance());
		
		glVertex2d(p[0],p[1]);
		glVertex2d(p[0]+0.1*std::cos(unit_->moveDirection())*unit_->viewDistance(),p[1]+0.1*std::sin(unit_->moveDirection())*unit_->viewDistance());		
		
		glEnd();
		/*
		// Draw small view sphere
		drawCircle(p[0],p[1],unit_->smallViewDistance(),20,false);
		*/
	}

	ZombieAnimation::ZombieAnimation(const UnitPtr& unit) : Task (1) {
		unit_ = unit;
	}

	void ZombieAnimation::excecute(double time) {
		draw(0.0);
	}

	bool ZombieAnimation::isRunning() const {
		return !unit_->isDead();
	}

	void ZombieAnimation::draw(double timestep) {
		Position p = unit_->getPosition();
		glColor3d(1,1,1);
		// Draw body		
		drawCircle(p[0],p[1],unit_->radius(),20,false);
		//drawCircle(p[0],p[1],unit_->radius()*0.5,20,false);

		glColor3d(1,1,1);
				
		// Draw view sphere
		//drawCircle(p[0],p[1],unit_->viewDistance(),20,false);
		glBegin(GL_LINES);
		//glVertex2d(p[0],p[1]);
		//glVertex2d(p[0]+std::cos(unit_->moveDirection() - 0.5*unit_->viewAngle())*unit_->viewDistance(),p[1]+std::sin(unit_->moveDirection() - 0.5*unit_->viewAngle())*unit_->viewDistance());
		//glVertex2d(p[0],p[1]);
		//glVertex2d(p[0]+std::cos(unit_->moveDirection() + 0.5*unit_->viewAngle())*unit_->viewDistance(),p[1]+std::sin(unit_->moveDirection() + 0.5*unit_->viewAngle())*unit_->viewDistance());
		
		double gg = unit_->radius();
		
		glVertex2d(p[0]-unit_->radius()*std::cos(unit_->moveDirection()-3.14/2),p[1]-unit_->radius()*std::sin(unit_->moveDirection()-3.14/2));
		glVertex2d(p[0]-unit_->radius()*std::cos(unit_->moveDirection()-3.14/2)+0.06*std::cos(unit_->moveDirection())*unit_->viewDistance(),p[1]-unit_->radius()*std::sin(unit_->moveDirection()-3.14/2)+0.06*std::sin(unit_->moveDirection())*unit_->viewDistance());		
		//glVertex2d(p[0]-unit_->radius()*std::cos(unit_->moveDirection()+3.14/2),p[1]-unit_->radius()*std::sin(unit_->moveDirection()+3.14/2));
		//glVertex2d(p[0]+0.04*std::cos(unit_->moveDirection())*unit_->viewDistance(),p[1]+0.04*std::sin(unit_->moveDirection())*unit_->viewDistance());		
		
		glVertex2d(p[0]-unit_->radius()*std::cos(unit_->moveDirection()+3.14/2),p[1]-unit_->radius()*std::sin(unit_->moveDirection()+3.14/2));
		glVertex2d(p[0]-unit_->radius()*std::cos(unit_->moveDirection()+3.14/2)+0.06*std::cos(unit_->moveDirection())*unit_->viewDistance(),p[1]-unit_->radius()*std::sin(unit_->moveDirection()+3.14/2)+0.06*std::sin(unit_->moveDirection())*unit_->viewDistance());		

		glEnd();
		/*
		// Draw small view sphere
		drawCircle(p[0],p[1],unit_->smallViewDistance(),20,false);
		*/
	}

	Shot::Shot(double x, double y, double currentTime) : Task (2) {
		startTime_ = currentTime;
		x_ = x;
		y_ = y;
		running_ = true;
	}

	void Shot::excecute(double time) {
		if (time < startTime_ + 2) {
			// Draw view sphere
			glColor3d(0,1,0);
			drawCircle(x_,y_,0.5,10,true);			
		} else {
			running_ = false;
		}
	}

	bool Shot::isRunning() const {
		return running_;
	}

	
	Death::Death(double x, double y, double currentTime) : Task (2) {
		startTime_ = currentTime;
		x_ = x;
		y_ = y;
		running_ = true;
	}

	void Death::excecute(double time) {
		if (time < startTime_ + 0.2) {
			// Draw view sphere
			glColor3d(1,0,0);
			drawCircle(x_,y_,(time-startTime_)*5,10,true);			
		} else {
			running_ = false;
		}
	}

	bool Death::isRunning() const {
		return running_;
	}

	BloodSplash::BloodSplash(double x, double y, double currentTime) : Task (2) {
		startTime_ = currentTime;
		x_ = x;
		y_ = y;
		running_ = true;
	}

	void BloodSplash::excecute(double time) {
		if (time < startTime_ + 0.1) {
			// Draw view sphere
			glColor3d(1,0,0);
			drawCircle(x_,y_,(time-startTime_)*3.5,10,true);			
		} else {
			running_ = false;
		}
	}

	bool BloodSplash::isRunning() const {
		return running_;
	}











} // Namespace zombie.