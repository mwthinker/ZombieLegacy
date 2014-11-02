#ifndef STATE_H
#define STATE_H

#include "box2ddef.h"

namespace zombie {

	class State {
	public:
		State() : position_(0, 0), velocity_(0, 0), angle_(0), anglularVelocity_(0) {
		}

		State(const Position& position, const Velocity& velocity, float angle = 0, float anglularVelocity = 0) :
			position_(position), velocity_(velocity), angle_(angle), anglularVelocity_(anglularVelocity) {
		}

		Position position_;
		Velocity velocity_;
		float angle_;
		float anglularVelocity_;
	};

} // Namespace zombie.

#endif // STATE_H
