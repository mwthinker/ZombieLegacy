#include "taskmanager.h"

#include "task.h"
#include "graphictask.h"

using namespace zombie;

TaskManager::TaskManager() {
	time_ = 0.0;
}

TaskManager::~TaskManager() {	
	for (Task* task : tasks_) {
		delete task;
	}

	for (GraphicTask* task : graphicTasks_) {
		delete task;
	}
}

void TaskManager::add(Task* task) {
	tasks_.push_back(task);
}

void TaskManager::add(GraphicTask* task) {
	graphicTasks_.push_back(task);
}

void TaskManager::update(double deltaTime) {
	// for each Task, call execute
	time_ += deltaTime;
	
	for (int i = 0; i < 3; ++i) {
		for (GraphicTask* task : graphicTasks_) {
			runGraphicTask(task,i);
		}
	}

	// Update all tasks.
	for (Task* task : tasks_) {
		if (task->isRunning()) {
			task->excecute(time_);
			Task* newTask = task->pull();
			if (newTask != 0) {
				// Add task to list.
				tasks_.push_back(newTask);
			}
		}
	}

	auto removeIfFunctionT = [] (Task* task) {
		// Is active?
		if (task->isRunning()) {			
			return false;
		}

		// Not active, delete and remove!
		delete task;
		return true;
	};

	auto removeIfFunctionG = [] (GraphicTask* task) {
		// Is active?
		if (task->isRunning()) {			
			return false;
		}

		// Not active, delete and remove!
 		delete task;
		return true;
	};

	// Remove dead tasks.
	tasks_.remove_if(removeIfFunctionT);
	graphicTasks_.remove_if(removeIfFunctionG);
}

void TaskManager::runGraphicTask(GraphicTask* task, int i) {
	if (task->isRunning()) {
		switch (i) {
		case 0:
			task->drawFirst(time_);
			break;
		case 1:
			task->drawSecond(time_);
			break;
		case 2:
			task->drawThird(time_);
			break;
		};
	}
}