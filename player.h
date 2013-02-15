#ifndef PLAYER_H
#define PLAYER_H

#include "input.h"
#include "unit.h"
#include "aibehavior.h"

#include <cmath>
#include <SDL.h>

namespace zombie {

class Player {
public:
	virtual ~Player() {
	}

	virtual Input currentInput() = 0;
};

} // namespace zombie.

#endif // PLAYER_H

