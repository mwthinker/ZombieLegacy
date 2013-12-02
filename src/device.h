#ifndef DEVICE_H
#define DEVICE_H

#include "input.h"
#include "getinput.h"

#include <memory>

union SDL_Event;

namespace zombie {

	class Device;
	typedef std::shared_ptr<Device> DevicePtr;

	class Device : public GetInput {
	public:
		virtual ~Device() {
		}

		virtual void eventUpdate(const SDL_Event& windowEvent) = 0;
	};

} // Namespace zombie.

#endif // DEVICE_H
