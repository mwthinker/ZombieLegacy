#ifndef STATE_H
#define STATE_H

#include "typedefs.h"

namespace zombie {

class State {
public:	
	Position position_;
	Velocity velocity_;
	float angle_;
	float anglularVelocity_;
};

} // namespace zombie.

#endif // STATE_H