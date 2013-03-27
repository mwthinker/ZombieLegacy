#ifndef ZOMBIEANIMAITON_H
#define ZOMBIEANIMAITON_H

#include "unit.h"
#include "graphicobject.h"
#include "color.h"

#include <vector>
#include <mw/sprite.h>

namespace zombie {

	class ZombieAnimation : public GraphicObject {
	public:
		ZombieAnimation(Unit* unit);
		~ZombieAnimation();

		void unitEventHandler(Unit::UnitEvent unitEvent);
		void draw(float timestep);
	
	private:
		mw::signals::Connection connection_;
		Unit* unit_;
		double timeNewFrame_;
		int index_;
		std::vector<mw::Sprite> sprites_;
		Color color_;
		float time_;
	};

} // Namespace zombie.

#endif //ZOMBIEANIMAITON_H