#ifndef ORDER_H
#define ORDER_H

#include "typedefs.h"

namespace zombie {

	class Order {
	public:
		Order();
		~Order();		
	private:
	};

	class Goto : Order {
	public:
		Goto();
		Goto(Position targetPos);
		~Goto();
	private:
		Position targetPos_;
	};

} // Namespace zombie.

#endif // ORDER_H
