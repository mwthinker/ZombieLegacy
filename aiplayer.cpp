#include "aiplayer.h"

#include "typedefs.h"
#include "input.h"
#include "zombiebehavior.h"

namespace zombie {

	AiPlayer::AiPlayer(AiBehaviorPtr behavior, const Unit* mOb) : Player(mOb), behavior_(behavior) {
	}	

	AiPlayer::AiPlayer(const Unit* mOb) : Player(mOb), behavior_(AiBehaviorPtr(new ZombieBehavior())) {
	}

	Input AiPlayer::currentInput() {
		return input_;
	}

	void AiPlayer::calculateInput(double time) {
		const Object* ob = Object::getObject(getId());
		if (ob != nullptr) {
			const Unit* unit = static_cast<const Unit*>(ob);
			input_ = behavior_->calculateInput(unit, time);
		}
	}

} // namespace zombie.
