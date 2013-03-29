#ifndef PLAYER_H
#define PLAYER_H

#include "input.h"
#include "player.h"

#include <vector>

namespace zombie {

	class Unit;

	class Player {
	public:
		virtual ~Player() {
		}

		virtual Input currentInput() = 0;
		virtual void calculateInput(Unit* unit, double time) = 0;
	};

} // namespace zombie.

#endif // PLAYER_H

