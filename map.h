#ifndef MAP_H
#define MAP_H

#include "aibehavior.h"
#include "unit.h"

#include <vector>
#include <random>

namespace zombie {

	// Generater used in all random operations.
	static std::default_random_engine generator_;

	static std::uniform_real_distribution<double> distribution_;

	class Map {
	public:
		Map() {
		}

		const std::vector<Position>& getSpawnSpots() const {
			return spawnSpots_;
		}

		BuildingPtr getBorder() const {
			return border_;
		}

		const std::vector<BuildingPtr>& getBuildings() const {
			return buildings_;
		}

		double getMapArea() const {
			return 1;
		}
	private:
		double polygonArea(const std::vector<Position>& corners) { 
			double area = 0;         // Accumulates area in the loop
			int size = corners.size();
			int j = size - 1;  // The last vertex is the 'previous' one to the first

			for (int i = 0; i < size; i++) {
				area = area + (corners[j].x_+corners[i].x_) * (corners[j].y_-corners[i].y_);
				j = i;  //j is previous vertex to i
			}
			return area/2;
		}

		std::vector<Position> spawnSpots_;
		std::vector<BuildingPtr> buildings_;
		BuildingPtr border_;
	};

	//std::default_random_engine generator_;

} // namespace zombie.

#endif // MAP_H


//std::default_random_engine generator_;