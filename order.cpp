

#include "order.h"
#include "typedefs.h"
#include "auxiliary.h"

#include <vector>


namespace zombie {

	Order::Order() {}
	Order::~Order() {}

	Goto::Goto(Position targetPos) {
		targetPos_ = targetPos;
	}
	
	Goto::~Goto() {}

}
