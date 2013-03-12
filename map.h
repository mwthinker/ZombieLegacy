#ifndef MAP_H
#define MAP_H

#include "typedefs.h"
#include "linefeature.h"

#include <mw/mathvector.h>

#include <vector>
#include <random>
#include <cmath>

namespace zombie {

	// Generater used in all random operations.
	static std::default_random_engine generator_;

	static std::uniform_real_distribution<float> distribution_;

	class Map {
	public:
		Map(Position mapCentre_, float width, float height, std::vector<BuildingPtr> buildings, std::vector<LineFeature> roads = std::vector<LineFeature>());
		Map(float tileSize, std::vector<BuildingPtr> buildings, std::vector<LineFeature> roads = std::vector<LineFeature>());
		Map();
				
		Position getMapCentre() const;
		Position generateSpawnPosition() const;
		Position generateSpawnPosition(Position p, float innerRadie, float outerRadie) const;

		const std::vector<BuildingPtr>& getBuildings() const {
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
		std::vector<BuildingPtr> buildings_;
	};

	Map generateMap(); 

	Map loadMapInfo(std::string filename, std::string fileRoads, float scale);

	Map loadTile(std::string filename, std::string fileRoads, float tileSize);

	std::vector<LineFeature> loadRoads(std::string filename, float scale, float normalizeX, float normalizeY);

} // namespace zombie.

/*
Map loadMap(std::string filename, int& unitId_) {
		std::fstream mapFile(filename.c_str(),std::fstream::in);
		
		if (mapFile.good()) {
			mapFile >> width_ >> height_;
		}
		
		while (mapFile.good()) {
			std::vector<Position> corners;
			while (mapFile.good()) {
				Position p;
				mapFile >> p.x_ >> p.y_;
				if (p.x_ < 0 || p.y_ < 0) {
					break;
				} else {
					corners.push_back(p);
				}
			}
			buildings_.push_back(BuildingPtr(new Building(corners,++unitId_)));
		}	
		
		mapFile.close();

		return Map(Position(width_*0.5,height_*0.5),std::min(width_,height_)*0.5,buildings_);
	}
*/

#endif // MAP_H


//std::default_random_engine generator_;