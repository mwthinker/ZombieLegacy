#ifndef DRIVER_H
#define DRIVER_H

#include "input.h"
#include "device.h"
#include "car.h"
#include "player.h"

namespace zombie {

	class Driver : public Player {
	public:
		Driver(DevicePtr device, Car* car) : Player(car), device_(device), car_(car) {
		}

		void updatePhysics(float time, float deltaTime) override {
			car_->updatePhysics(time, deltaTime, device_->nextInput());
		}

		MovingObject* getMovingObject() const override {
			return car_;
		}

	private:
		DevicePtr device_;
		Car* car_;
	};

} // Namespace zombie.

#endif // DRIVER_H
