#include "map.h"

#include "building.h"
#include "auxiliary.h"

#include <limits>
#include <string>
#include <fstream>

namespace zombie {

	Map::Map(Position mapCentre_, float width, float height, const std::vector<Points>& buildings, std::vector<LineFeature> roads) {
		for (const Points& points : buildings) {
			unsigned int size = points.size();
			// Is last and first point the same point?
			if (size > 0 && (points[size-1]-points[0]).LengthSquared() < 0.001f) {
				// Ignore last point.
				--size;
			}
			Points tmp(points.begin(), points.begin() + size);
			buildings_.push_back(new Building(tmp));
		}

		minX_ = mapCentre_.x - width * 0.5f;
		minY_ = mapCentre_.y - height * 0.5f;
		maxX_ = mapCentre_.x + width * 0.5f;
		maxY_ = mapCentre_.y + height * 0.5f;
		roads_ = roads;
	}

	Map::Map() {
	}

	Position Map::getMapCentre() const {
		return Position((minX_+maxX_)/2,(minY_+maxY_)/2);
	}

	Position Map::generateSpawnPosition() const {
		return generateSpawnPosition(getMapCentre(),0,std::min(maxX_-minX_,maxY_-minY_) * 0.7f);
	}

	Position Map::generateSpawnPosition(Position p, float innerRadie, float outerRadie) const {
		float alfa = random() * 2 * (float) mw::PI;
		float dist = random() * (outerRadie-innerRadie);

		for(float dr = 0; dr < outerRadie-innerRadie; dr += 1.0) {
			for(float angle = 0; angle <  (float) 2 * mw::PI; angle += 0.2f) {
				bool buildingFound = false;

				dist = fmod(dist+dr,outerRadie-innerRadie);
				float x = p.x + std::cos(alfa+angle)*(dist+innerRadie);
				float y = p.y + std::sin(alfa+angle)*(dist+innerRadie);

				for (Building* building : buildings_) {
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
		return Position(-99999,-99999);
	}

	Map generateMap() {
		float side = 15;
		int nbr = 10;

		std::vector<Points> buildings;

		for (int i = 0; i < nbr; ++i) {
			for (int j = 0; j < nbr; ++j) {

				float x = i * side;
				float y = j * side;
				Position p1(x + random() * 4, y + random() * 4);
				Position p2(x + random() * 4 + 8, y + random() * 4);
				Position p3(x + random() * 4 + 8, y + random() * 4 + 8);
				Position p4(x + random() * 4 , y + random() * 4 + 8);

				Points positions;
				positions.push_back(p1);
				positions.push_back(p2);
				positions.push_back(p3);
				positions.push_back(p4);

				buildings.push_back(positions);
			}
		}

		return Map(Position(),10,10,buildings);
	}

	Map loadMapInfo(std::string filename, std::string fileRoads, float scale) {
		std::fstream mapFile(filename.c_str(),std::fstream::in);
		float minX = std::numeric_limits<float>::max();
		float maxX = -minX;
		float minY = minX;
		float maxY = -minY;

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
					mapFile >> p.x >> p.y;
					// Scale map
					p.x = (p.x) * scale;
					p.y = (p.y) * scale;
					if (p.x < minX) {
						minX = p.x;
					}
					if (p.x > maxX) {
						maxX = p.x;
					}
					if (p.y < minY) {
						minY = p.y;
					}
					if (p.y > maxY) {
						maxY = p.y;
					}
					corners.push_back(p);
				}
				allCorners.push_back(corners);
			}
		}

		// Normalize map
		for (std::vector<Position>& corners : allCorners) {
			for (Position& corner : corners) {
				corner.x -= minX;
				corner.y -= minY;
			}
		}
		float normalizeX = minX;
		float normalizeY = minY;
		maxX -= normalizeX;
		minX -= normalizeX;
		maxY -= normalizeY;
		minY -= normalizeY;

		// Get world size.
		float height = maxX + minX;
		float width = maxY + minY;
		std::vector<Points> buildings;
		for (std::vector<Position>& corners : allCorners) {
			buildings.push_back(corners);
		}

		mapFile.close();
		return Map(Position((minX+maxX)/2,(minY+maxY)/2),width,height,buildings,loadRoads(fileRoads,scale,normalizeX,normalizeY));
	}

	std::vector<LineFeature> loadRoads(std::string filename, float scale, float normalizeX, float normalizeY) {
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
							mapFile >> p.x;
							mapFile >> p.y;
							p.x = p.x  * scale - normalizeX;
							p.y = p.y  * scale - normalizeY;
							vertexes.push_back(p);
						}

						int size = vertexes.size()-1;

						for (int i = 0; i < size; i++) {
							roads.push_back(LineFeature(vertexes[i],vertexes[i+1]));
						}
					}

					if (objectType == "LINE") {
						Position p1, p2;
						mapFile >> p1.x;
						mapFile >> p1.y;
						mapFile >> p2.x;
						mapFile >> p2.y;
						p1.x = p1.x * scale - normalizeX;
						p1.y = p1.y * scale - normalizeY;
						p2.x = p2.x * scale - normalizeX;
						p2.y = p2.y * scale - normalizeY;
						roads.push_back(LineFeature(p1,p2));
					} else {
						//std::cout << "no matching feature object type! ";
					}
				}
			}
		}

		return roads;
	}

	Map loadTile(std::string filename, std::string fileRoads, float tileSize) {
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
					mapFile >> p.x >> p.y;
					// Scale map
					p.x = (p.x);
					p.y = (p.y);
					corners.push_back(p);
				}
				allCorners.push_back(corners);
			}
		}
		std::vector<Points> buildings;
		for (std::vector<Position>& corners : allCorners) {
			buildings.push_back(corners);
		}

		mapFile.close();

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
							mapFile2 >> p.x;
							mapFile2 >> p.y;
							p.x = p.x;
							p.y = p.y;
							vertexes.push_back(p);
						}

						for (unsigned int i = 0; i < vertexes.size()-1; i++) {
							roads.push_back(LineFeature(vertexes[i],vertexes[i+1]));
						}
					}

					if (objectType == "LINE") {
						Position p1, p2;
						mapFile2 >> p1.x;
						mapFile2 >> p1.y;
						mapFile2 >> p2.x;
						mapFile2 >> p2.y;
						roads.push_back(LineFeature(p1,p2));
					} else {
						//std::cout << "no matching feature object type! ";
					}
				}
			}
		}

		return Map(Position(tileSize*0.5f,tileSize*0.5f),tileSize, tileSize, buildings, roads);
	}

} // namespace zombie.
