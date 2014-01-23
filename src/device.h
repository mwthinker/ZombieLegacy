#ifndef DEVICE_H
#define DEVICE_H

#include "input.h"

#include <SDL.h>

#include <memory>

namespace zombie {

	class Device;
	typedef std::shared_ptr<Device> DevicePtr;

	class Device {
	public:
		virtual ~Device() {
		}

		virtual void eventUpdate(const SDL_Event& windowEvent) = 0;

		virtual Input nextInput() = 0;
	};

} // Namespace zombie.

#endif // DEVICE_H
