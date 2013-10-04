#ifndef BLOOD_H
#define BLOOD_H

#include "typedefs.h"
#include "task.h"

#include <mw/sprite.h>

namespace zombie {

	class Blood : public Task {
	public:
		Blood(double x, double y, double currentTime);
		bool update(double time) override;

	private:
		double startTime_;
		double x_, y_;
		double startScaleX_;
		double startScaleY_;
		double endScaleX_;
		double endScaleY_;
		double duration_;

		mw::Sprite blood_;
	};

	class BloodStain : public Task {
	public:
		BloodStain(double x, double y, double currentTime);
		
		bool update(double time) override;

	private:
		double startTime_;
		double x_, y_;
		double scaleX_;
		double scaleY_;
		double duration_;
		double delay_;

		mw::Sprite blood_;
	};

	class BloodSplash : public Task {
	public:
		BloodSplash(double x, double y, double currentTime);
		
		bool update(double time) override;

	private:
		double startTime_;
		double x_, y_;
	};

} // Namespace zombie.

#endif // BLOOD_H
