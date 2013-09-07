#include "order.h"
#include "typedefs.h"

namespace zombie {

	Order::Order() {
	}
	
	Order::~Order() {
	}

	Goto::Goto(Position targetPos) {
		targetPos_ = targetPos;
	}
	
	Goto::~Goto() {
	}

} // Namespace zombie.
