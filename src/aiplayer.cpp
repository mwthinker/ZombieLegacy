#include "aiplayer.h"
#include "typedefs.h"
#include "input.h"
#include "zombiebehavior.h"
#include "unit.h"

namespace zombie {

	AiPlayer::AiPlayer(AiBehaviorPtr behavior, Unit* unit) : behavior_(behavior), unit_(unit) {
	}	

	AiPlayer::AiPlayer(Unit* unit) : behavior_(AiBehaviorPtr(new ZombieBehavior())), unit_(unit) {
	}

	void AiPlayer::updatePhysics(float time, float deltaTime) {
		unit_->updatePhysics(time, deltaTime, behavior_->calculateInput(unit_, time));
	}

	MovingObject* AiPlayer::getMovingObject() const {
		return unit_;
	}

} // Namespace zombie.
