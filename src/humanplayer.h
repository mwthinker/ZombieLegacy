#ifndef HUMANPLAYER_H
#define HUMANPLAYER_H

#include "player.h"
#include "device.h"
#include "unit.h"
#include "driver.h"

namespace zombie {

	class HumanPlayer : public Player {
	public:
		HumanPlayer(DevicePtr device, Unit* unit) : Player(unit), device_(device), unit_(unit) {
		}

		void updatePhysics(float time, float deltaTime) override {
			unit_->updatePhysics(time, deltaTime, device_->nextInput());
		}

		MovingObject* getMovingObject() const override {
			return unit_;
		}

		Driver* createDriver(Car* car) const override {
			return new Driver(device_, car);
		}

	private:
		DevicePtr device_;
		Unit* unit_;
	};

} // Namespace zombie.

#endif // HUMANPLAYER_H
