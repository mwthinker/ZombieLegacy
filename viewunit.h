#ifndef VIEWUNIT_H
#define VIEWUNIT_H

#include "typedefs.h"

namespace zombie {

	class ViewUnit {
	public:
		ViewUnit() {
		}

		ViewUnit(Position position, float angle, bool infected) {
			postion_ = position;
			angle_ = angle;
			inFected_ = infected;			
		}

		Position postion_;
		float angle_;
		bool inFected_;		
	};

} // Namespace zombie.

#endif // COLOR_H