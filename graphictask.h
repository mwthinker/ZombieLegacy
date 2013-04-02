#ifndef MAPTASK_H
#define MAPTASK_H

#include <vector>
#include <queue>
#include <SDL_opengl.h>
#include "map.h"
#include "gamesprite.h"

#include "unit.h"
#include "building.h"
#include "graphictask.h"
#include "color.h"
#include "weaponobject.h"

#include <mw/text.h>
#include <mw/signal.h>

namespace zombie {

	class GraphicTask {
	public:
		virtual ~GraphicTask();

		virtual void drawFirst(double time);
		virtual void drawSecond(double time);
		virtual void drawThird(double time);
		virtual bool isRunning() const = 0;
	};	

	class Shot : public GraphicTask {
	public:
		Shot(Position start, Position end, float startTime);
		void drawSecond(double time) override;		

		bool isRunning() const override;
	private:
		float startTime_;
		Position start_;
		Position end_;
		bool running_;
	};

	class Death : public GraphicTask {
	public:
		Death(double x, double y, double currentTime);
		void drawFirst(double time) override;

		bool isRunning() const override;
	private:
		double startTime_;
		double x_, y_;
		bool running_;
	};

	class MapDraw : public GraphicTask {
	public:
		MapDraw(const Map& map);
		void drawFirst(double time) override;		

		bool isRunning() const override;

	private:
		void draw();

		const Map& map_;
		mw::Sprite grass_;
	};

	class RoadDraw : public GraphicTask {
	public:
		RoadDraw(const Map& map);
		void drawFirst(double time) override;		

		bool isRunning() const override;
	private:
		void draw();

		const Map& map_;
		mw::Sprite road_;
	};

	class DrawFake3DBuildning : public GraphicTask {
	public:
		DrawFake3DBuildning(Building* building);
		void drawSecond(double time) override;
		void drawThird(double time) override;

		bool isRunning() const override;

	private:
		void draw();
		mw::Sprite road_;
		Building* buildning_;
		std::vector<LineFeature> front_;
		std::vector<LineFeature> back_;
		std::vector<LineFeature> rightCorner_;
		std::vector<LineFeature> leftCorner_;
		double height_;
		double d_;
		double r_,g_,b_;
	};

	unsigned int circularIndex(int i, int s);
	double getLineY(double a,double b,double c,double x);

	class DrawWeaponObject : public GraphicTask {
	private:
		DrawWeaponObject(WeaponObject* wOb) {

		}

		void drawFirst(double time) override {

		}

		bool isRunning() const override {

		}
	};

} // Namespace zombie.

#endif // MAPTASK_H