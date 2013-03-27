#ifndef HUMANANIMAITON_H
#define HUMANANIMAITON_H

#include "unit.h"
#include "graphicobject.h"
#include "color.h"

#include <vector>
#include <mw/sprite.h>

namespace zombie {

	class HumanAnimation : public GraphicObject {
	public:
		HumanAnimation(Unit* unit);
		~HumanAnimation();
		
		void unitEventHandler(Unit::UnitEvent unitEvent);
	
		void draw(float timestep);
	private:

		mw::signals::Connection connection_;
		Unit* unit_;
		double timeNewFrame_;
		int index_;
		std::vector<mw::Sprite> sprites_;
		double lastTime_;
		Color color_;
		float time_;
	};

} // Namespace zombie.

#endif //HUMANANIMAITON_H