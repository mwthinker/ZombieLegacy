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
		Map(Position mapCentre_, double width, double height, std::vector<BuildingPtr> buildings, std::vector<LineFeature> roads = std::vector<LineFeature>()) {
			buildings_ = buildings;
			minX_ = mapCentre_.x_ - width * 0.5; 
			minY_ = mapCentre_.y_ - height * 0.5;
			maxX_ = mapCentre_.x_ + width * 0.5;
			maxY_ = mapCentre_.y_ + height * 0.5;
			roads_ = roads;
		}

		Map() {
		}
				
		Position getMapCentre() const {
			return Position((minX_+maxX_)/2,(minY_+maxY_)/2);
		}		

		Position generateSpawnPosition() const {
			return generateSpawnPosition(getMapCentre(),0,std::min(maxX_-minX_,maxY_-minY_) * 0.7);
		}

		Position generateSpawnPosition(Position p, double innerRadie, double outerRadie) const {
			std::random_device rd;
			std::default_random_engine g(rd());
			std::uniform_real_distribution<double> distReal(0,1);
			bool buildingFound = false;

			double alfa = distReal(g) * 2 * mw::PI;
			double dist = distReal(g) * (outerRadie-innerRadie);// + innerRadie;			

			for(double dr = 0; dr < outerRadie-innerRadie; dr += 1.0) {
				for(double angle = 0; angle < 2*mw::PI; angle += 0.2) {
					dist = fmod(dist+dr,outerRadie-innerRadie);
					double x = p.x_ + std::cos(alfa+angle)*(dist+innerRadie);
					double y = p.y_ + std::sin(alfa+angle)*(dist+innerRadie);

					Position testPos(x,y);
					for (BuildingPtr building : buildings_) {
						// if inside one building, break
						if (building->isInside(testPos.x_,testPos.y_)) {
							buildingFound = true;
							break;
						}
					}

					if(!buildingFound) {
						return testPos;
					}
				}
			}
			// No success - return dummy position!
			return Position(-99999,-99999,-99999);
		}

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