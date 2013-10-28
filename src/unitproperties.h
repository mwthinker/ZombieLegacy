#ifndef UNITPROPERTIES_H
#define UNITPROPERTIES_H

#include "animation.h"

#include <string>

namespace zombie {

	class UnitProperties {
	public:
		std::string name_;
		float mass_;
		float radius_;
		float life_;
		float walkingSpeed_;
		float runningSpeed_;
		float stamina_;
		bool infected_;
		std::vector<TupleImageScaleTime> animation_;
	};

} // Namespace zombie.

#endif // UNITPROPERTIES_H
