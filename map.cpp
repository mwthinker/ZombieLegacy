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

		return Map(Position(),10,10,buildings);
	}	

	Map loadMapInfo(std::string filename, std::string fileRoads, int& unitId, double scale) {
		std::fstream mapFile(filename.c_str(),std::fstream::in);
		double minX = 99999999999999;
		double maxX = -99999999999999;
		double minY = 99999999999999;
		double maxY = -99999999999999;

		
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
					// Scale map
					p.x_ = (p.x_) * scale;
					p.y_ = (p.y_) * scale;
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

		// Normalize map		
		for (std::vector<Position>& corners : allCorners) {		
			for (Position& corner : corners) {		
				corner.x_ -= minX; 
				corner.y_ -= minY;
			}
		}
		double normalizeX = minX;
		double normalizeY = minY;
		maxX = maxX - minX;
		minX = 0;
		maxY = maxY - minY;		
		minY = 0;		

		// Get world size.
		double height = maxX + minX;
		double width = maxY + minY;
		std::vector<BuildingPtr> buildings;
		for (std::vector<Position>& corners : allCorners) {			
			BuildingPtr building = BuildingPtr(new Building(corners,++unitId));
			buildings.push_back(building);
		}
		
		mapFile.close();
		return Map(Position((minX+maxX)/2,(minY+maxY)/2),width,height,buildings,loadRoads(fileRoads,scale,normalizeX,normalizeY));
	}

	std::vector<LineFeature> loadRoads(std::string filename, double scale, double normalizeX, double normalizeY) {		
		std::vector<LineFeature> roads;
		std::fstream mapFile(filename.c_str(),std::fstream::in);
		while (mapFile.good()) {
			std::string line;
			getline (mapFile,line);
			
			if (line == "DATA") {
				while (mapFile.good()) {
					getline (mapFile,line);
					std::string objectType;
					mapFile >> objectType;
					
					if (objectType == "PLINE") {
						int nbrOfLines;
						mapFile >> nbrOfLines;
						std::vector<Position> vertexes;
						for (int i=1; i<=nbrOfLines;i++) {
							getline (mapFile,line);
							Position p;
							mapFile >> p.x_;
							mapFile >> p.y_;
							p.x_ = p.x_  * scale - normalizeX;
							p.y_ = p.y_  * scale - normalizeY;
							vertexes.push_back(p);
						}

						for (int i=0; i<vertexes.size()-1; i++) {
							roads.push_back(LineFeature(vertexes[i],vertexes[i+1]));	
						}
					}

					if (objectType == "LINE") {
						Position p1, p2;
						mapFile >> p1.x_;
						mapFile >> p1.y_;
						mapFile >> p2.x_;
						mapFile >> p2.y_;
						p1.x_ = p1.x_ * scale - normalizeX;
						p1.y_ = p1.y_ * scale - normalizeY;
						p2.x_ = p2.x_ * scale - normalizeX;
						p2.y_ = p2.y_ * scale - normalizeY;
						roads.push_back(LineFeature(p1,p2));
					} else {
						//std::cout << "no matching feature object type! ";
					}
				}
			}
		}
		
		std::cout << "Size roads_: "<< roads.size();
		//ladda in till roads attribut

		return roads;
	}

} // namespace zombie.

