#ifndef HUMANANIMAITON_H
#define HUMANANIMAITON_H

#include "unit.h"
#include "graphictask.h"

#include <vector>

namespace zombie {

	class HumanAnimation : public GraphicTask {
	public:
		HumanAnimation(Unit* unit);
		~HumanAnimation();
		void drawSecond(double time) override;		

		bool isRunning() const override;
		void unitEventHandler(Unit::UnitEvent unitEvent);

	private:
		void draw(double timestep);

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