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
		virtual bool update(double time) = 0;
	};

	void drawCircle(double cx, double cy, double r, int num_segments, bool filled);

}

#endif // TASK_H
