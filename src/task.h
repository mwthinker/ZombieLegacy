#ifndef TASK_H
#define TASK_H

namespace zombie {

	class Task {
	public:
		// The current size of the game windows.
		static int width, height;

		virtual ~Task() {
		}

		// Is called from the task manager, should perform a update, 
		// and time is the current time. The task must return true
		// in order to not be removed from the task manager.
		virtual bool update(float time) = 0;
	};	

}

#endif // TASK_H
