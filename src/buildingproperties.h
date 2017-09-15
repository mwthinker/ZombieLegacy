#ifndef BUILDINGPROPERTIES_H
#define BUILDINGPROPERTIES_H

#include "animation.h"

#include <mw/sound.h>

#include <string>

namespace zombie {

	struct MapProperties {
		enum ObjectType {BUILDING, WATER, GRASS, TILEPOINT, TREE, SPAWNINGPOINT, CAR_SPAWNINGPOINT};
		
		struct Object {
			ObjectType type_;
			std::vector<Position> geom_;
		};

		std::string name_;
		std::vector<Object> positions_;
	};

} // Namespace zombie.

#endif // BUILDINGPROPERTIES_H
