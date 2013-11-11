#include "aiplayer.h"
#include "typedefs.h"
#include "input.h"
#include "zombiebehavior.h"
#include "unit.h"

namespace zombie {

	AiPlayer::AiPlayer(AiBehaviorPtr behavior, Unit* unit) : Player(unit), behavior_(behavior), unit_(unit) {
	}	

	AiPlayer::AiPlayer(const Unit* unit) : Player(unit), behavior_(AiBehaviorPtr(new ZombieBehavior())) {
	}

	Input AiPlayer::currentInput() {
		return input_;
	}

	void AiPlayer::calculateInput(double time) {
		input_ = behavior_->calculateInput(unit_, time);
	}

} // Namespace zombie.
