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

		glColor3d(0,0.5,0);

		// Draw view sphere
		drawCircle(p[0],p[1],unit_->viewDistance(),20,false);
		glBegin(GL_LINES);
		glVertex2d(p[0],p[1]);
		glVertex2d(p[0]+std::cos(unit_->moveDirection() - 0.5*unit_->viewAngle())*unit_->viewDistance(),p[1]+std::sin(unit_->moveDirection() - 0.5*unit_->viewAngle())*unit_->viewDistance());
		glVertex2d(p[0],p[1]);
		glVertex2d(p[0]+std::cos(unit_->moveDirection() + 0.5*unit_->viewAngle())*unit_->viewDistance(),p[1]+std::sin(unit_->moveDirection() + 0.5*unit_->viewAngle())*unit_->viewDistance());
		glVertex2d(p[0],p[1]);
		glVertex2d(p[0]+std::cos(unit_->moveDirection())*unit_->viewDistance(),p[1]+std::sin(unit_->moveDirection())*unit_->viewDistance());		
		glEnd();

		// Draw small view sphere
		drawCircle(p[0],p[1],unit_->smallViewDistance(),20,false);
	}	

	TaskManager::TaskManager() {
		time_ = 0.0;
	}

	void TaskManager::add(Task* task) {
		tasks_.push_back(task);
	}

	void TaskManager::update(double deltaTime) {
		// for each Task, call execute
		time_ += deltaTime;

		for (Task* task : tasks_) {
			if (task->isRunning()) {
				task->excecute(time_);
				Task* newTask = task->pull();
				if (task != 0) {
					// Add task task list.
				}
			} else {
				// remove task.					
			}
		}
	}
}