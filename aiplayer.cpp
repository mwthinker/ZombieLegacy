#include "aiplayer.h"

#include "typedefs.h"
#include "input.h"
#include "zombiebehavior.h"

namespace zombie {

	AiPlayer::AiPlayer(AiBehaviorPtr behavior) : behavior_(behavior) {
	}	

	AiPlayer::AiPlayer() {
		behavior_ = AiBehaviorPtr(new ZombieBehavior());
	}

	Input AiPlayer::currentInput() {
		return input_;
	}

	void AiPlayer::calculateInput(Unit* unit, double time) {
		input_ = behavior_->calculateInput(unit, time);
	}
} // namespace zombie.