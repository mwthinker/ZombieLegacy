#ifndef AIBEHAVIOR_H
#define AIBEHAVIOR_H

#include "unit.h"

#include <vector>

namespace zombie {

	double calculateDifferenceBetweenAngles(double firstAngle, double secondAngle);

	double calculateAnglePointToPoint(double x, double y, double pX, double pY);

	class AiBehavior {
	public:
		AiBehavior();
		virtual ~AiBehavior();

		virtual Input calculateInput(const UnitPtr& unit, const std::vector<UnitPtr>& units, double time) = 0;	
	private:

	};

	class SimpleZombieBehavior : public AiBehavior {
	public:
		SimpleZombieBehavior();

		Input calculateInput(const UnitPtr& unit, const std::vector<UnitPtr>& units, double time) override;	
	private: 
		int timeBeforeInputChange_;
		Input current_;
		double time_;

	};

	class BoringZombieBehavior : public AiBehavior {
	public:
		BoringZombieBehavior();

		Input calculateInput(const UnitPtr& unit, const std::vector<UnitPtr>& units, double time) override;

	private: 
		int timeBeforeInputChange_;
		Input current_;
		double time_;

	};

	class AgressiveZombieBehavior : public AiBehavior {
	public:
		AgressiveZombieBehavior();

		Input calculateInput(const UnitPtr& unit, const std::vector<UnitPtr>& units, double time) override;	

		bool setTarget(const std::vector<UnitPtr>& units);

		bool validateTarget();

		bool targetLocked();

		bool randomWalk();
	private: 
		int timeBeforeInputChange_;
		UnitPtr target_;
		Input current_;
		double time_;

	};

	typedef std::shared_ptr<AiBehavior> AiBehaviorPtr;

} // namespace zombie.

#endif // AIBEHAVIOR_H