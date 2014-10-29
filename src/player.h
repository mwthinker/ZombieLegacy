#ifndef PLAYER_H
#define PLAYER_H

#include "input.h"

namespace zombie {

	class Player {
	public:
		virtual void updateInput(float time, float deltaTime) = 0;
	};

} // Namespace zombie.

#endif // PLAYER_H
