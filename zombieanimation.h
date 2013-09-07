#ifndef ZOMBIEANIMAITON_H
#define ZOMBIEANIMAITON_H

#include "unit.h"
#include "graphictask.h"
#include "typedefs.h"

#include <vector>
#include <mw/sprite.h>

namespace zombie {

	class ZombieAnimation : public GraphicTask {
	public:
		ZombieAnimation(Unit* unit);
		~ZombieAnimation();

		void unitEventHandler(Unit::UnitEvent unitEvent);
		
		bool draw(double time) override;
	
	private:
		mw::signals::Connection connection_;

		int idUnit_;
		double timeNewFrame_;
		int index_;
		std::vector<mw::Sprite> sprites_;
		Color color_;
	};

} // Namespace zombie.

#endif //ZOMBIEANIMAITON_H
