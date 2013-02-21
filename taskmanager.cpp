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

tasks_.sort([] (Task* task1, Task* task2 ) {
		// Is active?
		
		


		if (task1->getDrawOrder() <= task2->getDrawOrder()) {			
			return false;
		}
		return true;
	});
}

void TaskManager::update(double deltaTime) {
	// for each Task, call execute
	time_ += deltaTime;

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

	// Remove dead tasks.
	tasks_.remove_if([] (Task* task) {
		// Is active?
		if (task->isRunning()) {			
			return false;
		}

		// Not active, delete and remove!
		delete task;
		return true;
	});
}