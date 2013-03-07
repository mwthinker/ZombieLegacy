#ifndef TASK_H
#define TASK_H

#include <vector>
#include <queue>
#include <SDL_opengl.h>
#include "map.h"
#include "gamesprite.h"

#include "unit.h"
#include "building.h"

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
		void unitEventHandler(Unit::UnitEvent unitEvent);

	private:
		void draw(double timestep);

		UnitPtr unit_;
		double timeNewFrame_;
		int index_;
		std::vector<mw::Sprite> sprites_;
		bool walk_;
		double lastTime_;
	};

	class ZombieAnimation : public Task {
	public:
		ZombieAnimation(const UnitPtr& unit);
		void excecute(double time) override;		
		bool isRunning() const override;
		void unitEventHandler(Unit::UnitEvent unitEvent);

	private:
		void draw(double timestep);

		UnitPtr unit_;
		double timeNewFrame_;
		int index_;
		std::vector<mw::Sprite> sprites_;
		bool walk_;
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

	class Buildning3DTask : public Task {
	public:
		Buildning3DTask(const BuildingPtr& building);
		void excecute(double time) override;		
		bool isRunning() const override;

	private:
		void draw();

		BuildingPtr buildning_;
	};

	class MapDraw : public Task {
	public:
		MapDraw(const Map& map);
		void excecute(double time) override;		
		bool isRunning() const override;

	private:
		void draw();

		const Map& map_;
		mw::Sprite grass_;
	};


	class RoadDraw : public Task {
	public:
		RoadDraw(const Map& map);
		void excecute(double time) override;		
		bool isRunning() const override;

	private:
		void draw();

		const Map& map_;
		mw::Sprite road_;
	};

	class DrawFake3DBuildning : public Task {
	public:
		DrawFake3DBuildning(const BuildingPtr& building);
		void excecute(double time) override;		
		bool isRunning() const override;

	private:
		void draw();
		mw::Sprite road_;
		BuildingPtr buildning_;
		double height_;
		double d_;
		double r_,g_,b_;
};

unsigned int circularIndex(int i, int s);

}



#endif // TASK_H