#ifndef MAP_H
#define MAP_H

#include "aibehavior.h"
#include "unit.h"
#include "building.h"
#include "linefeature.h"

#include <vector>
#include <random>
#include <cmath>

namespace zombie {

	// Generater used in all random operations.
	static std::default_random_engine generator_;

	static std::uniform_real_distribution<double> distribution_;

	class Map {
	public:
		Map(Position mapCentre_, double width, double height, std::vector<BuildingPtr> buildings, std::vector<LineFeature> roads = std::vector<LineFeature>());
		Map();
				
		Position getMapCentre() const;
		Position generateSpawnPosition() const;
		Position generateSpawnPosition(Position p, double innerRadie, double outerRadie) const;

		const std::vector<BuildingPtr>& getBuildings() const {
			return buildings_;
		}

		const std::vector<LineFeature> getRoads() const {
			return roads_;
		}

		double minX() const {
			return minX_;
		}

		double minY() const {
			return minY_;
		}

		double maxX() const {
			return maxX_;
		}

		double maxY() const {
			return maxY_;
		}

		double width() const {
			return maxX_ - minX_;
		}

		double height() const {
			return maxY_ - minY_;
		}

	private:
		double minX_;
		double minY_;
		double maxX_;
		double maxY_;
		
		std::vector<LineFeature> roads_;
		std::vector<BuildingPtr> buildings_;
	};

	Map generateMap(int& lastId); 

	Map loadMapInfo(std::string filename, std::string fileRoads, int& unitId, double scale);

	std::vector<LineFeature> loadRoads(std::string filename, double scale, double normalizeX, double normalizeY);

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