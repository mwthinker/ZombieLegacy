#ifndef HUMANSTATUS_H
#define HUMANSTATUS_H

#include "task.h"

#include <mw/text.h>

namespace zombie {

	class Unit;

	class HumanStatus : public Task {
	public:
		enum PlayerNumber {ONE, TWO, THREE, FOUR};

		HumanStatus(Unit* unit, PlayerNumber player);

		void excecute(double time) override;
		bool isRunning() const override;

	private:
		void draw(double timestep);

		int unitId_;
		double lastTime_;
		mw::Text name_;
		mw::Text life_;
		mw::Text ammo_;
		PlayerNumber player_;
	};

}

#endif // HUMANSTATUS_H
