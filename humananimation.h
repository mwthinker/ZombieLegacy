#ifndef HUMANANIMAITON_H
#define HUMANANIMAITON_H

#include "unit.h"
#include "graphictask.h"
#include "color.h"

#include <vector>
#include <mw/sprite.h>

namespace zombie {

	class HumanAnimation : public GraphicTask {
	public:
		HumanAnimation(Unit* unit);
		~HumanAnimation();
		
		void unitEventHandler(Unit::UnitEvent unitEvent);
	
		void drawSecond(double time) override;
		bool isRunning() const override;

	private:
		InMemory inMemory_;
		mw::signals::Connection connection_;
		Unit* unit_;
		double timeNewFrame_;
		int index_;
		std::vector<mw::Sprite> sprites_;
		double lastTime_;
		Color color_;
	};

} // Namespace zombie.

#endif //HUMANANIMAITON_H
