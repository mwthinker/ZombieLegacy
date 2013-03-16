#ifndef BLOOD_H
#define BLOOD_H

#include "typedefs.h"
#include "graphictask.h"

namespace zombie {

	class Blood : public GraphicTask {
	public:
		Blood(double x, double y, double currentTime);
		void drawFirst(double time) override;

		bool isRunning() const override;
		double getScaleX(double dT) const;
		double getScaleY(double dT) const;

	private:
		double startTime_;
		double x_, y_;
		double startScaleX_;
		double startScaleY_;
		double endScaleX_;
		double endScaleY_;
		double duration_;
		bool running_;
		mw::Sprite blood_;
	};

	class BloodStain : public GraphicTask {
	public:
		BloodStain(double x, double y, double currentTime);
		void drawFirst(double time) override;		

		bool isRunning() const override;

	private:
		double startTime_;
		double x_, y_;
		double scaleX_;
		double scaleY_;
		double duration_;
		double delay_;
		bool running_;
		mw::Sprite blood_;
	};

	class BloodSplash : public GraphicTask {
	public:
		BloodSplash(double x, double y, double currentTime);
		void drawFirst(double time) override;

		bool isRunning() const override;

	private:
		double startTime_;
		double x_, y_;
		bool running_;
	};

} // Namespace zombie.

#endif // BLOOD_H
