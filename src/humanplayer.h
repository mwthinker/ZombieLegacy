#ifndef HUMANPLAYER_H
#define HUMANPLAYER_H

#include "player.h"
#include "device.h"
#include "unit.h"

namespace zombie {

	class HumanPlayer : public Player {
	public:
		HumanPlayer(DevicePtr device, Unit* unit) : Player(unit), device_(device), unit_(unit) {
		}

		void updatePhysics(float time, float deltaTime) override {
			unit_->updatePhysics(time, deltaTime, device_->currentInput());
		}

		MovingObject* getMovingObject() const override {
			return unit_;
		}

	private:
		DevicePtr device_;
		Unit* unit_;
	};

} // Namespace zombie.

#endif // HUMANPLAYER_H
