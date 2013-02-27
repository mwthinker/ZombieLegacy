#ifndef TASKMANAGER_H
#define TASKMANAGER_H

#include <list>
#include <utility>

namespace zombie {

	class Task;

	class TaskManager {
	public:
		TaskManager();
		~TaskManager();

		// Adds task to be managed in ascending order based on the level.
		void add(Task* task, int level);

		// Updates each task in ascending order.
		void update(double deltaTime);
	private:
		typedef std::pair<int,Task*> Pair;
		std::list<Pair> tasks_;
		double time_;
	};

} // Namespace zombie.

#endif // TASKMANAGER_H