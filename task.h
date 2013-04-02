#ifndef TASK_H
#define TASK_H

#include <vector>
#include <queue>
#include <SDL_opengl.h>
#include "map.h"
#include "gamesprite.h"

#include "unit.h"
#include "building.h"
#include "weaponobject.h"

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
		enum Player {ONE, TWO, THREE, FOUR};

		HumanStatus(Unit* unit, Player player);
		void excecute(double time) override;
		bool isRunning() const override;

	private:
		void draw(double timestep);

		Unit* unit_;
		double lastTime_;
		mw::Text name_;
		mw::Text life_;
		mw::Text ammo_;
		Player player_;
	};

}
#endif // TASK_H