#ifndef ZOMBIEBEHAVIOR_H
#define ZOMBIEBEHAVIOR_H

#include "input.h"
#include "player.h"
#include "box2ddef.h"

#include <mw/signal.h>
#include <list>

namespace zombie {

	class Unit;
	class MovingObject;

	class ZombieBehavior : public Player {
	public:
		ZombieBehavior(Unit* unit);
		~ZombieBehavior();

		void updatePhysics(float time, float deltaTime) override;

		MovingObject* getMovingObject() const override;

	private:
		MovingObject* findUninfectedTarget(Position position, const std::list<MovingObject*>& units) const;

		void targetUpdate(MovingObject* mOb);

		double findNewTargetTime_;
		double timeToUpdateAngleDirection_;
		double targetAngle_;
		bool forward_;

		Position targetPosition_;

		mw::signals::Connection targetConnection_;

		Unit* unit_;
	};

} // Namespace zombie.

#endif // ZOMBIEBEHAVIOR_H
