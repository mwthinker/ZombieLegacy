#ifndef HUMANPLAYER_H
#define HUMANPLAYER_H

#include "player.h"
#include "device.h"

namespace zombie {

	class HumanPlayer : public Player {
	public:
		HumanPlayer(DevicePtr device, const MovingObject* mOb) : Player(mOb), device_(device) {
		}

		void calculateInput(double time) override {
		}

		Input currentInput() override {
			return device_->currentInput();
		}

	private:
		DevicePtr device_;
	};

} // namespace zombie.

#endif // HUMANPLAYER_H
