#include "map.h"

#include "building.h"

#include <limits>
#include <string>
#include <fstream>

namespace zombie {

	Map::Map(Position mapCentre_, double width, double height, std::vector<BuildingPtr> buildings, std::vector<LineFeature> roads) {
		buildings_ = buildings;
		minX_ = mapCentre_.x_ - width * 0.5; 
		minY_ = mapCentre_.y_ - height * 0.5;
		maxX_ = mapCentre_.x_ + width * 0.5;
		maxY_ = mapCentre_.y_ + height * 0.5;
		roads_ = roads;
	}

	Map::Map(double tileSize, std::vector<BuildingPtr> buildings, std::vector<LineFeature> roads) {
		buildings_ = buildings;
		minX_ = 0;
		minY_ = 0;
		maxX_ = tileSize;
		maxY_ = tileSize;
		roads_ = roads;
	}

	Map::Map() {
	}

	Position Map::getMapCentre() const {
		return Position((minX_+maxX_)/2,(minY_+maxY_)/2);
	}		

	Position Map::generateSpawnPosition() const {
		return generateSpawnPosition(getMapCentre(),0,std::min(maxX_-minX_,maxY_-minY_) * 0.7);
	}

	Position Map::generateSpawnPosition(Position p, double innerRadie, double outerRadie) const {
		std::random_device rd;
		std::default_random_engine g(rd());
		std::uniform_real_distribution<double> distReal(0,1);		

		double alfa = distReal(g) * 2 * mw::PI;
		double dist = distReal(g) * (outerRadie-innerRadie);// + innerRadie;			

		for(double dr = 0; dr < outerRadie-innerRadie; dr += 1.0) {
			for(double angle = 0; angle < 2*mw::PI; angle += 0.2) {
				bool buildingFound = false;

				dist = fmod(dist+dr,outerRadie-innerRadie);
				double x = p.x_ + std::cos(alfa+angle)*(dist+innerRadie);
				double y = p.y_ + std::sin(alfa+angle)*(dist+innerRadie);
								
				for (BuildingPtr building : buildings_) {
					// if inside one building, break
					if (building->isInside(x,y)) {
						buildingFound = true;
						break;
					}
				}

				// Spot free from buildings?
				if (!buildingFound) {
					return Position(x,y);
				}
			}
		}
		// No success - return dummy position!
		return Position(-99999,-99999,-99999);
	}

	Map generateMap() {
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

				BuildingPtr building(new Building(positions));
				buildings.push_back(building);
			}
		}

		return Map(Position(),10,10,buildings);
	}	

	Map loadMapInfo(std::string filename, std::string fileRoads, double scale) {
		std::fstream mapFile(filename.c_str(),std::fstream::in);
		double minX = std::numeric_limits<double>::max();
		double maxX = -minX;
		double minY = minX;
		double maxY = -minY;

		std::vector< std::vector<Position> > allCorners;
		while (mapFile.good()) {
			//while (mapFile.good()) {
			std::string line;
			getline (mapFile,line);
			if (line == "REGION 1") {
				std::vector<Position> corners;
				// Extract all points
				int nbrLines;
				mapFile >> nbrLines;	
				//std::cout<<"REGION " << nbrLines;
				for (int i = 1; i <= nbrLines; i++) {
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
		maxX -= normalizeX;
		minX -= normalizeX;
		maxY -= normalizeY;		
		minY -= normalizeY;		

		// Get world size.
		double height = maxX + minX;
		double width = maxY + minY;
		std::vector<BuildingPtr> buildings;
		for (std::vector<Position>& corners : allCorners) {			
			BuildingPtr building = BuildingPtr(new Building(corners));
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
						for (int i = 1; i <= nbrOfLines; i++) {
							getline (mapFile,line);
							Position p;
							mapFile >> p.x_;
							mapFile >> p.y_;
							p.x_ = p.x_  * scale - normalizeX;
							p.y_ = p.y_  * scale - normalizeY;
							vertexes.push_back(p);
						}

						for (unsigned int i = 0; i < vertexes.size()-1; i++) {
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

		return roads;
	}


	Map loadTile(std::string filename, std::string fileRoads, double tileSize) {
		std::fstream mapFile(filename.c_str(),std::fstream::in);
		
		std::vector< std::vector<Position> > allCorners;
		while (mapFile.good()) {
			//while (mapFile.good()) {
			std::string line;
			getline (mapFile,line);
			if (line == "REGION 1") {
				std::vector<Position> corners;
				// Extract all points
				int nbrLines;
				mapFile >> nbrLines;	
				//std::cout<<"REGION " << nbrLines;
				for (int i = 1; i <= nbrLines; i++) {
					Position p;
					mapFile >> p.x_ >> p.y_;
					// Scale map
					p.x_ = (p.x_);
					p.y_ = (p.y_);
					corners.push_back(p);
				}
				allCorners.push_back(corners);					
			}
		}
		std::vector<BuildingPtr> buildings;
		for (std::vector<Position>& corners : allCorners) {			
			BuildingPtr building = BuildingPtr(new Building(corners));
			buildings.push_back(building);
		}

		mapFile.close();

		//********
		std::vector<LineFeature> roads;
		std::fstream mapFile2(fileRoads.c_str(),std::fstream::in);
		while (mapFile2.good()) {
			std::string line;
			getline (mapFile2,line);

			if (line == "DATA") {
				while (mapFile2.good()) {
					getline (mapFile2,line);
					std::string objectType;
					mapFile2 >> objectType;

					if (objectType == "PLINE") {
						int nbrOfLines;
						mapFile2 >> nbrOfLines;
						std::vector<Position> vertexes;
						for (int i = 1; i <= nbrOfLines; i++) {
							getline (mapFile2,line);
							Position p;
							mapFile2 >> p.x_;
							mapFile2 >> p.y_;
							p.x_ = p.x_;
							p.y_ = p.y_ ;
							vertexes.push_back(p);
						}

						for (unsigned int i = 0; i < vertexes.size()-1; i++) {
							roads.push_back(LineFeature(vertexes[i],vertexes[i+1]));	
						}
					}

					if (objectType == "LINE") {
						Position p1, p2;
						mapFile2 >> p1.x_;
						mapFile2 >> p1.y_;
						mapFile2 >> p2.x_;
						mapFile2 >> p2.y_;
						roads.push_back(LineFeature(p1,p2));
					} else {
						//std::cout << "no matching feature object type! ";
					}
				}
			}
		}
		//************

		return Map(tileSize, buildings, roads);

	
	
	
	}

} // namespace zombie.
