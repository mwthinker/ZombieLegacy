#ifndef SURVIVALTIMER_H
#define SURVIVALTIMER_H

#include "task.h"
#include "typedefs.h"

#include <vector>

namespace zombie {

	class SurvivalTimer : public Task {
	public:
		SurvivalTimer() {
			dayTime_ = 60.0;
		}

		void excecute(double time) override {
		}

		bool isRunning() const override {
			return true;
		}
	private:
		double dayTime_;
	};

} // namespace zombie.

#endif // SURVIVALTIMER_H