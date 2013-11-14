#ifndef TASKMANAGER_H
#define TASKMANAGER_H

#include <list>

namespace zombie {

	class Task;

	enum GraphicLevel {
		GROUND_LEVEL,
		ROAD_LEVEL,
		TREE_LEVEL,
		BUILDING_LEVEL,
		BLOOD_LEVEL,
		WEAPON_LEVEL,
		SHOT_LEVEL,
		HEAVEN_LEVEL,
		INTERFACE_LEVEL
	};

	class TaskManager {
	public:
		TaskManager();
		~TaskManager();

		// Adds task to be managed in ascending order based on the level.
		// The ownership now belongs to TaskManager.
		void add(Task* task, GraphicLevel level);

		// Updates each task in ascending order. Tasks may been removed
		// from memory after the execution of this function.
		void update(double deltaTime);
		
		// Remove all tasks from internal list and from memory.
		void clear();
	
	private:
		typedef std::pair<Task*, GraphicLevel> Pair;
		std::list<Pair> tasks_;

		double time_;
	};

} // Namespace zombie.

#endif // TASKMANAGER_H
