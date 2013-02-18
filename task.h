#ifndef TASK_H
#define TASK_H

#include <vector>
#include <queue>
#include <SDL_opengl.h>

#include "unit.h"
#include "building.h"

#include <mw/text.h>

#include <memory>

namespace zombie {

	typedef std::shared_ptr<Unit> UnitPtr;

	class Task {
	public:
		static int width, height;

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
		void excecute(double time) override;		
		bool isRunning() const override;

	private:
		void draw();

		BuildingPtr buildning_;
	};

	class HumanAnimation : public Task {
	public:
		HumanAnimation(const UnitPtr& unit);
		void excecute(double time) override;
		bool isRunning() const override;

	private:
		void draw(double timestep);

		UnitPtr unit_;
	};

	class ZombieAnimation : public Task {
	public:
		ZombieAnimation(const UnitPtr& unit);
		void excecute(double time) override;		
		bool isRunning() const override;

	private:
		void draw(double timestep);

		UnitPtr unit_;
	};

	class SurvivorAnimation : public Task {
	public:
		SurvivorAnimation(const UnitPtr& unit);
		void excecute(double time) override;		
		bool isRunning() const override;

	private:
		void draw(double timestep);

		UnitPtr unit_;
	};
	

	class Shot : public Task {
	public:
		Shot(double x, double y, double currentTime);
		void excecute(double time) override;
		bool isRunning() const override;

	private:
		double startTime_;
		double x_, y_;
		bool running_;
	};

	class Death : public Task {
	public:
		Death(double x, double y, double currentTime);
		void excecute(double time) override;
		bool isRunning() const override;

	private:
		double startTime_;
		double x_, y_;
		bool running_;
	};

	class BloodSplash : public Task {
	public:
		BloodSplash(double x, double y, double currentTime);
		void excecute(double time) override;
		bool isRunning() const override;

	private:
		double startTime_;
		double x_, y_;
		bool running_;
	};

	class HumanAnimation3D : public Task {
	public:
		HumanAnimation3D(const UnitPtr& unit);
		void excecute(double time) override;
		bool isRunning() const override;

	private:
		void draw(double timestep);

		UnitPtr unit_;
		double lastTime_;
	};

	class HumanStatus : public Task {
	public:
		enum Player {ONE, TWO, THREE, FOUR};

		HumanStatus(const UnitPtr& unit, Player player);
		void excecute(double time) override;
		bool isRunning() const override;

	private:
		void draw(double timestep);

		UnitPtr unit_;
		double lastTime_;
		mw::Text name_;
		mw::Text life_;
		mw::Text ammo_;
		Player player_;
	};
}

#endif // TASK_H