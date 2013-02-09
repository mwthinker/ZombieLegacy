#ifndef ZOMBIEBEHAVIOR_H
#define ZOMBIEBEHAVIOR_H

#include "aibehavior.h"
#include "unit.h"

#include <vector>
#include <random>

namespace zombie {

	// Generater used in all random operations.
	static std::default_random_engine generator_;

	class ZombieBehavior : public AiBehavior {
	public:
		ZombieBehavior() {
			findNewTargetTime_ = distribution_(generator_) * 3;
			timeToUpdateAngleDirection_ = distribution_(generator_) * 1;
			targetAngle_ = distribution_(generator_) * mw::PI * 2;
			forward_ = true;
		}

		~ZombieBehavior() {
		}

		Input calculateInput(const UnitPtr& unit, const std::vector<UnitPtr>& units, double time) {
			Input input;

			if (time > findNewTargetTime_) {
				findNewTargetTime_ = distribution_(generator_) * 3 + time;
				target_ = findUninfectedTarget(unit->getPosition(), units);				
			}

			if (time > timeToUpdateAngleDirection_) {
				timeToUpdateAngleDirection_ = distribution_(generator_) * 1 + time;

				// Has a target?
				if (target_ != nullptr) {
					mw::MathVector dir = target_->getPosition() - unit->getPosition();
					targetAngle_ = std::atan2(dir.y_,dir.x_);
					forward_ = true;
				} else {
					targetAngle_ += (distribution_(generator_)-0.5)*2 * mw::PI * 2 * 0.1;
					forward_ = distribution_(generator_) > 0.25;
				}
			}			

			double diffAngle = calculateDifferenceBetweenAngles(targetAngle_,unit->moveDirection());
			
			// Turn?
			if (std::abs(diffAngle) > 0.1 ) {
				if (diffAngle > 0) {
					input.turnLeft_ = true;
				} else {
					input.turnRight_ = true;
				}
			}

			input.forward_ = forward_;

			return input;
		}
	private:
		UnitPtr findUninfectedTarget(Position position, const std::vector<UnitPtr>& units) const {
			UnitPtr target(nullptr);
			double distant = 100;
			for (const UnitPtr& unit : units) {
				// Not infected?
				if (!unit->isInfected()) {
					double tmp = (position - unit->getPosition()).magnitudeSquared();					
					// Closer?
					if (tmp < distant) {
						target = unit;
						distant = tmp;
					}
				}
			}

			return target;
		}

		double findNewTargetTime_;
		UnitPtr target_;

		double timeToUpdateAngleDirection_;
		double targetAngle_;
		bool forward_;

		static std::uniform_real_distribution<double> distribution_;
		//int index = distribution(generator_);
	};

} // namespace zombie.

#endif // ZOMBIEBEHAVIOR_H


//std::default_random_engine generator_;
