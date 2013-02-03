#include "taskmanager.h"

#include "task.h"

using namespace zombie;

TaskManager::TaskManager() {
	time_ = 0.0;
}

TaskManager::~TaskManager() {	
	for (Task* task : tasks_) {
		delete task;
	}
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