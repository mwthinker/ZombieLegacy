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
		Map(Position mapCentre_, double radius, std::vector<BuildingPtr> buildings,double normalizeX, double normalizeY) {
			border_ = std::shared_ptr<Border>(new Border(mapCentre_, radius));
			buildings_ = buildings;
			area_ = radius*radius*mw::PI;
			minX_ = mapCentre_.x_ - radius; 
			minY_ = mapCentre_.y_ - radius;
			maxX_ = mapCentre_.x_ + radius;
			maxY_ = mapCentre_.y_ + radius;
			road_.push_back(Position(10,10));
			road_.push_back(Position(25,25));
			normalizeX_ = normalizeX;
			normalizeY_ = normalizeY;
		}

		Map(Position mapCentre_, double radius, std::vector<BuildingPtr> buildings) {
			border_ = std::shared_ptr<Border>(new Border(mapCentre_, radius));
			buildings_ = buildings;
			area_ = radius*radius*mw::PI;
			minX_ = mapCentre_.x_ - radius; 
			minY_ = mapCentre_.y_ - radius;
			maxX_ = mapCentre_.x_ + radius;
			maxY_ = mapCentre_.y_ + radius;
			road_.push_back(Position(10,10));
			road_.push_back(Position(25,25));
		}

		Map() {
			area_ = 0;
		}

		Position getMapCentre() const {
			return border_->getCentrePosition();
		}

		Position generateSpawnPosition() const {
			return generateSpawnPosition(border_->getCentrePosition(),0,border_->getRadius()/2);
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

		std::shared_ptr<Border> getBorder() const {
			return border_;
		}

		const std::vector<BuildingPtr>& getBuildings() const {
			return buildings_;
		}
		
		void loadRoads(std::string filename, double scale);

		//const std::vector<Position>& getRoads() const {
		//	return road_;
		//}

		const std::vector<LineFeature> getRoads() const {
			return roads_;
		}

		double getMapArea() const {
			return area_;
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
		std::shared_ptr<Border> border_;
		/*
		static double polygonArea(const std::vector<Position>& corners) { 
			double area = 0;         // Accumulates area in the loop
			int size = corners.size();
			int j = size - 1;  // The last vertex is the 'previous' one to the first

			for (int i = 0; i < size; i++) {
				area = area + (corners[j].x_+corners[i].x_) * (corners[j].y_-corners[i].y_);
				j = i;  //j is previous vertex to i
			}
			return area/2;
		}
		*/
				
		std::vector<BuildingPtr> buildings_;
		std::vector<Position> road_; // DUMMY IMPLEMENTATION REMOVE
		double area_;
		double minX_;
		double minY_;
		double maxX_;
		double maxY_;
		double normalizeX_;
		double normalizeY_;
		std::vector<LineFeature> roads_;
	};
	Map generateMap(int& lastId); 

	Map loadMapInfo(std::string filename, int& unitId, double scale);

	//std::default_random_engine generator_;

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