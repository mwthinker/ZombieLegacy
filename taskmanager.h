#ifndef TASKMANAGER_H
#define TASKMANAGER_H

#include <list>

namespace zombie {

	class Task;
	class GraphicTask;

	class TaskManager {
	public:
		TaskManager();
		~TaskManager();

		// Adds task to be managed in ascending order based on the level.
		void add(Task* task);
		void add(GraphicTask* task);

		// Updates each task in ascending order.
		void update(double deltaTime);
	
	private:
		std::list<Task*> tasks_;
		std::list<GraphicTask*> graphicTasks_;

		double time_;
	};

} // Namespace zombie.

#endif // TASKMANAGER_H