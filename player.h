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

class AiPlayer : public Player {
public:
	AiPlayer(AiBehaviorPtr behavior);

	AiPlayer();

	Input currentInput();

	void calculateInput(const UnitPtr& unit, double time);
	void updateUnitsInView(const std::vector<UnitPtr>& units);

	Input input_;
	AiBehaviorPtr behavior_;
	std::vector<UnitPtr> units_;
};

} // namespace zombie.

#endif // LOCALPLAYER_H

