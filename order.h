#ifndef ORDER_H
#define ORDER_H

#include "typedefs.h"
#include "auxiliary.h"

#include <vector>


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

}

#endif // ORDER_H