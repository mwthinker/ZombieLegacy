#ifndef AIPLAYER_H
#define AIPLAYER_H

#include "aibehavior.h"
#include "input.h"
#include "player.h"

namespace zombie {

	class Unit;

	class AiPlayer : public Player {
	public:
		AiPlayer(AiBehaviorPtr behavior, Unit* unit);
		AiPlayer(Unit* unit);

		void updatePhysics(float time, float deltaTime) override;

		MovingObject* getMovingObject() const override;
		
	private:
		AiBehaviorPtr behavior_;
		Unit* unit_;
	};

} // Namespace zombie.

#endif // AIPLAYER_H
