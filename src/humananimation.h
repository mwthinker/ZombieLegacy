#ifndef HUMANANIMAITON_H
#define HUMANANIMAITON_H

#include "task.h"
#include "unit.h"
#include "animation.h"

#include <vector>
#include <mw/sprite.h>
#include <mw/signals/connection.h>

namespace zombie {

	class HumanAnimation : public Task {
	public:
		HumanAnimation(Unit* unit, const Animation& animation);
		~HumanAnimation();
		
		void unitEventHandler(Unit::UnitEvent unitEvent);
	
		bool update(double time) override;

	private:
		mw::signals::Connection connection_;
		int humanId_;

		Animation animation_;
	};

} // Namespace zombie.

#endif //HUMANANIMAITON_H
