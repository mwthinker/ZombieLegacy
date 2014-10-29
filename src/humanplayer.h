#ifndef HUMANPLAYER_H
#define HUMANPLAYER_H

#include "device.h"
#include "unit.h"
#include "player.h"

namespace zombie {

	class HumanPlayer : public Player {
	public:
		HumanPlayer(DevicePtr device, Unit* unit) : device_(device), unit_(unit) {
		}

		void updateInput(float time, float deltaTime) override {
			unit_->setInput(device_->nextInput());
		}

		Unit* getUnit() const {
			return unit_;
		}

	private:
		DevicePtr device_;
		Unit* unit_;
	};

} // Namespace zombie.

#endif // HUMANPLAYER_H
