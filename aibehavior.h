#ifndef AIBEHAVIOR_H
#define AIBEHAVIOR_H

#include "typedefs.h"

#include "input.h"
#include <vector>

namespace zombie {

	class AiBehavior {
	public:
		AiBehavior();
		virtual ~AiBehavior();

		virtual Input calculateInput(UnitPtr unit, const std::vector<Unit*>& units, double time) = 0;	
	private:

	};

	class SimpleZombieBehavior : public AiBehavior {
	public:
		SimpleZombieBehavior();

		Input calculateInput(UnitPtr unit, const std::vector<Unit*>& units, double time) override;	
	private: 
		int timeBeforeInputChange_;
		Input current_;
		double time_;

	};

	class BoringZombieBehavior : public AiBehavior {
	public:
		BoringZombieBehavior();

		Input calculateInput(UnitPtr unit, const std::vector<Unit*>& units, double time) override;

	private: 
		int timeBeforeInputChange_;
		Input current_;
		double time_;

	};

	class AgressiveZombieBehavior : public AiBehavior {
	public:
		AgressiveZombieBehavior();

		Input calculateInput(UnitPtr unit, const std::vector<Unit*>& units, double time) override;	

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

} // namespace zombie.

#endif // AIBEHAVIOR_H