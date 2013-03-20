#ifndef TASKMANAGER_H
#define TASKMANAGER_H

#include <mw/quadtree.h>

#include <list>
#include <utility>

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
		void add(GraphicTask* task, double x, double y, double width, double height);
		void add(GraphicTask* task, double x, double y, double radius);

		// Updates each task in ascending order.
		void update(double deltaTime);
	private:
		void runGraphicTask(GraphicTask* task, int i);

		std::list<Task*> tasks_;
		mw::Quadtree<GraphicTask*> graphicMaptasks_;
		std::list<GraphicTask*> graphicTasks_;

		double time_;
	};

} // Namespace zombie.

#endif // TASKMANAGER_H