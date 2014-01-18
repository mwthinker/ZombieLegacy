#ifndef BUILDINGPROPERTIES_H
#define BUILDINGPROPERTIES_H

#include "box2ddef.h"

namespace zombie {

	class BuildingProperties {
	public:
		std::vector<Point> points_;
		float height_;
	};

} // Namespace zombie.

#endif // BUILDINGPROPERTIES_H
