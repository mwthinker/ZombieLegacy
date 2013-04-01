#ifndef MAP_H
#define MAP_H

#include "typedefs.h"
#include "linefeature.h"

#include <mw/mathvector.h>

#include <vector>
#include <cmath>

namespace zombie {

	class Building;

	class Map {
	public:
		Map(Position mapCentre_, float width, float height, const std::vector<Points>& buildings, std::vector<LineFeature> roads = std::vector<LineFeature>());
		
		Map();
				
		Position getMapCentre() const;
		Position generateSpawnPosition() const;
		Position generateSpawnPosition(Position p, float innerRadie, float outerRadie) const;

		const std::vector<Building*>& getBuildings() const {
			return buildings_;
		}

		const std::vector<LineFeature>& getRoads() const {
			return roads_;
		}

		float minX() const {
			return minX_;
		}

		float minY() const {
			return minY_;
		}

		float maxX() const {
			return maxX_;
		}

		float maxY() const {
			return maxY_;
		}

		float width() const {
			return maxX_ - minX_;
		}

		float height() const {
			return maxY_ - minY_;
		}
	private:
		float minX_;
		float minY_;
		float maxX_;
		float maxY_;
		
		std::vector<LineFeature> roads_;
		std::vector<Building*> buildings_;
	};

	Map generateMap(); 

	Map loadMapInfo(std::string filename, std::string fileRoads, float scale);

	Map loadTile(std::string filename, std::string fileRoads, float tileSize);

	std::vector<LineFeature> loadRoads(std::string filename, float scale, float normalizeX, float normalizeY);

} // namespace zombie.

#endif // MAP_H
