#ifndef STATE_H
#define STATE_H

#include "typedefs.h"

namespace zombie {

class State {
public:	
	Position position_;
	Velocity velocity_;
	double angle_;
	double angleVelocity_;
};

} // namespace zombie.

#endif // STATE_H