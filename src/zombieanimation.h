#ifndef ZOMBIEANIMAITON_H
#define ZOMBIEANIMAITON_H

#include "unit.h"
#include "task.h"
#include "typedefs.h"
#include "animation.h"

#include <vector>
#include <mw/sprite.h>

namespace zombie {

	class ZombieAnimation : public Task {
	public:
		ZombieAnimation(Unit* unit, const Animation& animation);
		~ZombieAnimation();

		void unitEventHandler(Unit::UnitEvent unitEvent);
		
		bool update(double time) override;
	
	private:
		mw::signals::Connection connection_;

		int idUnit_;
		Animation animation_;
	};

} // Namespace zombie.

#endif //ZOMBIEANIMAITON_H
