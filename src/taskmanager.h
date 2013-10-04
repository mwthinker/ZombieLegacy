#ifndef TASKMANAGER_H
#define TASKMANAGER_H

#include <list>
#include <utility>

namespace zombie {

	class Task;
	class GraphicTask;

	enum GraphicLevel {
		GROUND_LEVEL,
		ROAD_LEVEL,
		TREE_LEVEL,
		BUILDING_LEVEL,
		BLOOD_LEVEL,
		WEAPON_LEVEL,
		SHOT_LEVEL,
		UNIT_LEVEL,
		HEAVEN_LEVEL,
		INTERFACE_LEVEL
	};

	class TaskManager {
	public:
		TaskManager();
		~TaskManager();

		// Adds task to be managed in ascending order based on the level.
		void add(Task* task);
		void add(GraphicTask* task, GraphicLevel level);

		// Updates each task in ascending order.
		void update(double deltaTime);
	
	private:
		std::list<Task*> tasks_;
		typedef std::pair<GraphicTask*, GraphicLevel> Pair;
		std::list<Pair> graphicTasks_;

		double time_;
	};

} // Namespace zombie.

#endif // TASKMANAGER_H
