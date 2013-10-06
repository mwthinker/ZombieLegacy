#ifndef HUMANANIMAITON_H
#define HUMANANIMAITON_H

#include "task.h"
#include "unit.h"

#include <vector>
#include <mw/sprite.h>
#include <mw/signals/connection.h>

namespace zombie {

	class HumanAnimation : public Task {
	public:
		HumanAnimation(Unit* unit);
		~HumanAnimation();
		
		void unitEventHandler(Unit::UnitEvent unitEvent);
	
		bool update(double time) override;

	private:
		mw::signals::Connection connection_;
		int humanId_;

		double timeNewFrame_;
		int index_;
		std::vector<mw::Sprite> sprites_;

		double lastTime_;
		Color color_;
	};

} // Namespace zombie.

#endif //HUMANANIMAITON_H