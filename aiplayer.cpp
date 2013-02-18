#include "aiplayer.h"
#include "input.h"
#include "unit.h"
#include "zombiebehavior.h"

namespace zombie {

	AiPlayer::AiPlayer(AiBehaviorPtr behavior) : behavior_(behavior) {
	}	

	AiPlayer::AiPlayer() {
		//behavior_ = AiBehaviorPtr(new SimpleZombieBehavior());
		//behavior_ = AiBehaviorPtr(new AgressiveZombieBehavior());
		//behavior_ = AiBehaviorPtr(new BoringZombieBehavior());
		behavior_ = AiBehaviorPtr(new ZombieBehavior());
		
	}

	Input AiPlayer::currentInput() {
		return input_;
	}

	void AiPlayer::calculateInput(const UnitPtr& unit, double time) {
		input_ = behavior_->calculateInput(unit, units_, time);
	}

	void AiPlayer::updateUnitsInView(const std::vector<UnitPtr>& units) {
		units_ = units;
	}

} // namespace zombie.