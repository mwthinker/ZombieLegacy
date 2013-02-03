#ifndef TASKMANAGER_H
#define TASKMANAGER_H

#include <vector>

namespace zombie {

	class Task;

	class TaskManager {
	public:
		TaskManager();
		~TaskManager();

		void add(Task* task);
		void update(double deltaTime);
	private:
		std::vector<Task*> tasks_;
		double time_;
	};

} // Namespace zombie.

#endif // TASKMANAGER_H