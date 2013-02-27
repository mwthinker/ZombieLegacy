#ifndef ZOMBIEBEHAVIOR_H
#define ZOMBIEBEHAVIOR_H

#include "aibehavior.h"
#include "typedefs.h"
#include "auxiliary.h"

#include <vector>
#include <random>

namespace zombie {

	// Generater used in all random operations.
	static std::default_random_engine generator_;

	class ZombieBehavior : public AiBehavior {
	public:
		ZombieBehavior();
		~ZombieBehavior();

		Input calculateInput(const UnitPtr& unit, const std::vector<UnitPtr>& units, double time) override;

	private:
		UnitPtr findUninfectedTarget(Position position, const std::vector<UnitPtr>& units) const;

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
