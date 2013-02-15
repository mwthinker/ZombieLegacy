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
			return Position();
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

	Map generateMap(int& lastId) {
		double side = 15;
		int nbr = 10;

		std::default_random_engine g;
		std::uniform_int_distribution<int> distInt(4,7);
		std::uniform_real_distribution<double> distReal(0,1);

		std::vector<BuildingPtr> buildings;
		
		for (int i = 0; i < nbr; ++i) {
			for (int j = 0; j < nbr; ++j) {
				
				double x = i * side;
				double y = j * side;
				Position p1(x + distReal(g) * 4, y + distReal(g) * 4, 0);
				Position p2(x + distReal(g) * 4 + 8, y + distReal(g) * 4, 0);
				Position p3(x + distReal(g) * 4 + 8, y + distReal(g) * 4 + 8, 0);
				Position p4(x + distReal(g) * 4 , y + distReal(g) * 4 + 8, 0);
				
				std::vector<Position> positions;
				positions.push_back(p1);
				positions.push_back(p2);
				positions.push_back(p3);
				positions.push_back(p4);

				BuildingPtr building(new Building(positions, ++lastId));
				buildings.push_back(building);
			}
		}

		return Map(Position(),10,buildings);
	}

	//std::default_random_engine generator_;

} // namespace zombie.

#endif // MAP_H


//std::default_random_engine generator_;