#include "map.h"
#include <string>
#include <fstream>
#include "input.h"

namespace zombie {

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

	Map loadMapInfo(std::string filename, int& unitId) {
		std::fstream mapFile(filename.c_str(),std::fstream::in);
		double minX = 99999999999999;
		double maxX = -99999999999999;
		double minY = 99999999999999;
		double maxY = -99999999999999;

		if (mapFile.good()) {
			//mapFile >> width_ >> height_;
		}
		std::vector< std::vector<Position> > allCorners;
		while (mapFile.good()) {
			//while (mapFile.good()) {
			std::string line;
			getline (mapFile,line);
			if(line == "REGION 1") {
				std::vector<Position> corners;
				// Extract all points
				int nbrLines;
				mapFile >> nbrLines;	
				//std::cout<<"REGION " << nbrLines;
				for (int i=1; i <= nbrLines; i++) {
					Position p;
					mapFile >> p.x_ >> p.y_;
					if (p.x_ < minX) {
						minX = p.x_;
					}
					if (p.x_ > maxX) {
						maxX = p.x_;
					}
					if (p.y_ < minY) {
						minY = p.y_;
					}
					if (p.y_ > maxY) {
						maxY = p.y_;
					}
					corners.push_back(p);
				}
				allCorners.push_back(corners);					
			}
		}

		// GET WORLD SIZE
			double height = maxX - minX;
			double width = maxY - minY;
			std::vector<BuildingPtr> buildings;
		for (std::vector<Position>& corners : allCorners) {			
			BuildingPtr building = BuildingPtr(new Building(corners,++unitId));
			buildings.push_back(building);
		}
		
				
		
		
		mapFile.close();
		return Map(Position((minX+maxX)/2,(minY+maxY)/2),std::min(height,width)/2,buildings);
	}

} // namespace zombie.

