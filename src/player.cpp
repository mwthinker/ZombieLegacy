#include "player.h"
#include "movingobject.h"

namespace zombie {

	Player::Player(MovingObject* mOb) {
		mOb->player_ = this;
	}

} // Namespace zombie.
