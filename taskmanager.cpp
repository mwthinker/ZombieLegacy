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

	// Delete all graphic tasks.
	for (Pair pair : graphicTasks_) {
		delete pair.first;
	}
}

void TaskManager::add(Task* task) {
	tasks_.push_back(task);
}

void TaskManager::add(GraphicTask* task, GraphicLevel level) {
	Pair pair(task, level);
	auto it = graphicTasks_.begin();
	for (;it != graphicTasks_.end(); ++it) {
		if (it->second > level) {
			break;
		}
	}
	graphicTasks_.insert(it,pair);
}

void TaskManager::update(double deltaTime) {
	// for each Task, call execute
	time_ += deltaTime;

	// Draw all tasks in order and remove the non drawable.
	graphicTasks_.remove_if([&] (const Pair& pair) {
		// Is active?
		if (pair.first->draw(time_)) {
			return false;
		}

		// Not active, delete and remove!
		delete pair.first;
		return true;
	});

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

	// Remove dead tasks.
	tasks_.remove_if(removeIfFunctionT);
}
