#ifndef TASK_H
#define TASK_H

#include <queue>

#include "unit.h"

#include <mw/text.h>

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

	class HumanStatus : public Task {
	public:
		enum PlayerNumber {ONE, TWO, THREE, FOUR};

		HumanStatus(Unit* unit, PlayerNumber player);
		void excecute(double time) override;
		bool isRunning() const override;

	private:
		void draw(double timestep);

		int unitId_;
		double lastTime_;
		mw::Text name_;
		mw::Text life_;
		mw::Text ammo_;
		PlayerNumber player_;
	};

}
#endif // TASK_H