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

		void calculateInput(Unit* unit, double time);
		void updateUnitsInView(const std::vector<Unit*>& units);

		Input input_;
		AiBehaviorPtr behavior_;
		std::vector<Unit*> units_;
	};

} // namespace zombie.

#endif // AIPLAYER_H