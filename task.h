#ifndef TASK_H
#define TASK_H

#include <vector>
#include <queue>
#include <SDL_opengl.h>

#include "unit.h"
#include "building.h"

#include <memory>

namespace zombie {

	typedef std::shared_ptr<Unit> UnitPtr;

	class Task {
	public:
		Task(int drawOrder);

		virtual void excecute(double time) = 0;
		virtual bool isRunning() const = 0;		
		
		int getDrawOrder() const;

		Task* pull();
	protected:
		void push(Task* task);

	private:
		std::queue<Task*> tasks_;
		int drawOrder_;
	};

	void drawCircle(double cx, double cy, double r, int num_segments, bool filled);

	class DrawBuildning : public Task {
	public:
		DrawBuildning(const BuildingPtr& building);
		void excecute(double time);
		
		bool isRunning() const;
	private:
		void draw();

		BuildingPtr buildning_;

	};

	class HumanAnimation : public Task {
	public:
		HumanAnimation(const UnitPtr& unit);

		void excecute(double time);
		
		bool isRunning() const;

	private:
		void draw(double timestep);

		UnitPtr unit_;
	};

	class DeathAnimation : Task {	
	};

	class Shot : public Task {
	public:
		Shot(double x, double y, double currentTime);

		void excecute(double time);
		bool isRunning() const;
	private:
		double startTime_;
		double x_, y_;
		bool running_;
	};
}

#endif // TASK_H