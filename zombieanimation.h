#ifndef ZOMBIEANIMAITON_H
#define ZOMBIEANIMAITON_H

#include "unit.h"
#include "graphictask.h"

#include <vector>

namespace zombie {

	class ZombieAnimation : public GraphicTask {
	public:
		ZombieAnimation(Unit* unit);
		~ZombieAnimation();

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
		Color color_;
	};

} // Namespace zombie.

#endif //ZOMBIEANIMAITON_H