#ifndef TASK_H
#define TASK_H

#include <queue>

namespace zombie {

	class Task {
	public:
		static int width, height;

		virtual void excecute(double time) = 0;
		virtual bool isRunning() const = 0;
		Task* pull();

	protected:
		void push(Task* task);

	private:
		std::queue<Task*> tasks_;
	};

	void drawCircle(double cx, double cy, double r, int num_segments, bool filled);

}

#endif // TASK_H
