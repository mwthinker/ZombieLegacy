#ifndef MAP_H
#define MAP_H

#include "aibehavior.h"
#include "unit.h"
#include "building.h"

#include <vector>
#include <random>

namespace zombie {

	// Generater used in all random operations.
	static std::default_random_engine generator_;

	static std::uniform_real_distribution<double> distribution_;

	class Map {
	public:
		Map(Position mapCentre_, double radius, std::vector<BuildingPtr> buildings) {
			border_ = std::shared_ptr<Border>(new Border(mapCentre_, radius));
			buildings_ = buildings;
			area_ = radius*radius*mw::PI;
		}

		Map() {
			area_ = 0;
		}

		Position getMapCentre() const {
			return border_->getCentrePosition();
		}

		Position generateSpawnPosition() const {
			return Position();
		}

		Position generateSpawnPosition(Position p, double innerRadie, double outerRadie) const {
			std::default_random_engine g;
			std::uniform_real_distribution<double> distReal(0,1);
			
			for(int tries = 0; tries < 100; tries++) {
				double alfa = distReal(g) * 2 * mw::PI;
				double dist = distReal(g) * (outerRadie-innerRadie) + innerRadie;
				double x = p.x_ + std::cos(alfa)*dist;
				double y = p.y_ + std::sin(alfa)*dist;
				Position testPos(x,y);
				for (BuildingPtr building : buildings_) {
					// if inside one building, break
					if(building->isInside(testPos.x_,testPos.y_)) {
						break;
					// Passed all buildings -> return position!
					} else if (buildings_.back() == building){ 
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

		double getMapArea() const {
			return area_;
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
		double area_;
	};
	Map generateMap(int& lastId); 

	Map loadMapInfo(std::string filename);

	//std::default_random_engine generator_;

} // namespace zombie.

#endif // MAP_H


//std::default_random_engine generator_;