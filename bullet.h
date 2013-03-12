#ifndef BULLET_H
#define BULLET_H

#include "typedefs.h"

namespace zombie {

	struct Bullet {
	public:
		Position postion_;
		float damage_;
		float direction_;
		float range_;	
	};

} // Namespace zombie.

#endif // BULLET_H