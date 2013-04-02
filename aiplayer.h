#ifndef AIPLAYER_H
#define AIPLAYER_H

#include "typedefs.h"
#include "input.h"
#include "player.h"

#include <vector>

namespace zombie {

	class Unit;

	class AiPlayer : public Player {
	public:
		AiPlayer(AiBehaviorPtr behavior);

		AiPlayer();

		Input currentInput() override;

		void calculateInput(const Unit* unit, double time);

		Input input_;
		AiBehaviorPtr behavior_;
	};

} // namespace zombie.

#endif // AIPLAYER_H