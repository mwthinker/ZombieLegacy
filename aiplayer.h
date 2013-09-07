#ifndef AIPLAYER_H
#define AIPLAYER_H

#include "aibehavior.h"
#include "input.h"
#include "player.h"

namespace zombie {

	class Unit;

	class AiPlayer : public Player {
	public:
		AiPlayer(AiBehaviorPtr behavior, const Unit* mOb);

		AiPlayer(const Unit* mOb);

		Input currentInput() override;

		void calculateInput(double time) override;

		Input input_;
		AiBehaviorPtr behavior_;
	};

} // Namespace zombie.

#endif // AIPLAYER_H
