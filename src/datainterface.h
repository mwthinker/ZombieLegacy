#ifndef DATAINTERFACE_H
#define DATAINTERFACE_H

#include "box2ddef.h"
#include "animation.h"
#include "weapon.h"

#include "mw/color.h"

#include <vector>
#include <string>

namespace zombie {

	class DataInterface {
	public:
		// Map.
		virtual void loadTree(const Position& position) = 0;
		
		virtual void loadBuilding(const std::vector<Position>& corners) = 0;
		
		virtual void loadRoad(const std::vector<Position>& road) = 0;
		
		virtual void loadWater(const std::vector<Position>& positions) = 0;
		
		virtual void loadSpawningPoint(const Position& position) = 0;
	
	protected:
		// To avoid virtual destructor.
		~DataInterface() {
		}
	};

} // Namespace zombie.

#endif // DATAINTERFACE_H
