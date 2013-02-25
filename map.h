#ifndef MAP_H
#define MAP_H

#include "aibehavior.h"
#include "unit.h"
#include "building.h"

#include <vector>
#include <random>
#include "quadtree.h"

namespace zombie {

	// Generater used in all random operations.
	static std::default_random_engine generator_;

	static std::uniform_real_distribution<double> distribution_;

	class Map {
	public:
		Map(Position mapCentre_, double radius, std::vector<BuildingPtr> buildings) {
			border_ = std::shared_ptr<Border>(new Border(mapCentre_, radius));
			area_ = radius*radius*mw::PI;
			Position lowerLeft = mapCentre_ - radius*Position(1,1);
			spawnPositions_ = Quadtree<Position>(lowerLeft.x_,lowerLeft.y_,radius*2,radius*2,5);
			buildings_ = Quadtree<BuildingPtr>(lowerLeft.x_,lowerLeft.y_,radius*2,radius*2,5);
			for (auto& building : buildings) {				
				Position p = building->getPosition() - building->getRadius()*Position(1,1);
				buildings_.add(building, p.x_,p.y_, building->getRadius()*2, building->getRadius()*2);
			}
			initSpawnPositions(lowerLeft,radius*2,radius*2);
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
			std::uniform_real_distribution<double> distReal(-1,1);

			bool spawnPositionFound = false;

			Position spawn;

			std::vector<Position> positions;

			while (!spawnPositionFound) {
				positions = spawnPositions_.getObjectsAt(p.x_,p.y_,outerRadie);
				std::uniform_int_distribution<int> distInt(0,positions.size());
				int index = distInt(g);
				int size = positions.size();
				for (int i = 0; i < size; ++i) {
					Position tmp = positions[(index+i) % size];
					// Is outside innerradie?
					if ((tmp - p).magnitudeSquared() > innerRadie*innerRadie) {
						spawn = tmp;
						spawnPositionFound = true;
						break;
					}
				}
				outerRadie += 5;
			}
			spawn += Position(distReal(g),distReal(g));
			return spawn;
		}

		std::shared_ptr<Border> getBorder() const {
			return border_;
		}

		const std::vector<BuildingPtr>& getBuildings() const {
			return buildings_.getContainer();
		}

		double getMapArea() const {
			return area_;
		}
	private:
		void initSpawnPositions(Position lowerLeft, double width, double height) {
			double stepSize = 3;

			double x = lowerLeft.x_ + width*0.1;
			while (x < lowerLeft.x_ + width*0.9) {
				double y = lowerLeft.y_ + height*0.1;
				while (y < lowerLeft.y_ + height*0.9) {			
					auto buildings = buildings_.getObjectsAt(x,y,stepSize,stepSize);

					bool inside = false;
					for (BuildingPtr building : buildings) {
						// if inside one building, break
						if(building->isInside(x,y)) {
							inside = false;
							break;
						}
					}
					if (true || !inside) {
						spawnPositions_.add(Position(x,y),x,y,1,1);
					}

					y += stepSize;
				}
				x += stepSize;
			}
		}

		std::shared_ptr<Border> border_;
		Quadtree<Position> spawnPositions_;

		Quadtree<BuildingPtr> buildings_;
		double area_;
	};
	Map generateMap(int& lastId); 

	Map loadMapInfo(std::string filename, int& unitId, double scale);

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
	//std::default_random_engine generator_;

} // namespace zombie.

#endif // MAP_H


//std::default_random_engine generator_;