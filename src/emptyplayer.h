#ifndef EMPTYPLAYER_H
#define EMPTYPLAYER_H

#include "input.h"
#include "car.h"
#include "player.h"

namespace zombie {

	class EmptyPlayer : public Player {
	public:
		EmptyPlayer(Car* car) : Player(car), car_(car) {
		}

		void updatePhysics(float time, float deltaTime) override {
			car_->updatePhysics(time, deltaTime, Input());
		}

		MovingObject* getMovingObject() const override {
			return car_;
		}

	private:
		Car* car_;
	};

} // Namespace zombie.

#endif // EMPTYPLAYER_H
