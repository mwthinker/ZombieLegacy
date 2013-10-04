#include "taskmanager.h"
#include "task.h"

#include <utility>

namespace zombie {

	TaskManager::TaskManager() {
		time_ = 0.0;
	}

	TaskManager::~TaskManager() {
		// Delete all graphic tasks.
		for (Pair pair : tasks_) {
			delete pair.first;
		}
	}

	void TaskManager::add(Task* task, GraphicLevel level) {
		Pair pair(task, level);
		auto it = tasks_.begin();
		for (;it != tasks_.end(); ++it) {
			if (it->second > level) {
				break;
			}
		}
		tasks_.insert(it,pair);
	}

	void TaskManager::update(double deltaTime) {
		// for each Task, call execute
		time_ += deltaTime;

		// Update all tasks in order and remove all which returns false.
		tasks_.remove_if([&] (const Pair& pair) {
			// Is active?
			if (pair.first->update(time_)) {
				return false;
			}

			// Not active, delete and remove!
			delete pair.first;
			return true;
		});
	}

} // Namespace zombie.
