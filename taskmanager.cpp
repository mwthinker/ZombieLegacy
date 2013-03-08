#include "taskmanager.h"

#include "task.h"
#include "maptask.h"

using namespace zombie;

TaskManager::TaskManager() {
	time_ = 0.0;
}

TaskManager::~TaskManager() {	
	for (Pair& pair : tasks_) {
		delete pair.second;
	}
}

void TaskManager::add(Task* task, int level) {
	auto it = tasks_.begin();
	for (; it != tasks_.end(); ++it) {
		if (level < it->first) {
			break;
		}
	}

	tasks_.insert(it, Pair(level,task));
}

void TaskManager::update(double deltaTime) {
	// for each Task, call execute
	time_ += deltaTime;

	for (Pair& pair : tasks_) {
		if (pair.second->isRunning()) {
			pair.second->excecute(time_);
			Task* newTask = pair.second->pull();
			if (newTask != 0) {
				// Add task to list.
				tasks_.push_back(Pair(pair.first,newTask));
			}
		}
	}

	// Remove dead tasks.
	tasks_.remove_if([] (const Pair& pair) {
		// Is active?
		if (pair.second->isRunning()) {			
			return false;
		}

		// Not active, delete and remove!
		delete pair.second;
		return true;
	});
}