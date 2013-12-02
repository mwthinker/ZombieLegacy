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
			Unit* unit = car_->getDriver();
			if (unit != nullptr) {
				//unit->getPlayer()->
			} else {
				car_->updatePhysics(time, deltaTime, Input());
			}
		}

		MovingObject* getMovingObject() const override {
			return car_;
		}

		virtual GetInputPtr get() const {
			return nullptr;
		}

	private:
		Car* car_;
	};

} // Namespace zombie.

#endif // EMPTYPLAYER_H
