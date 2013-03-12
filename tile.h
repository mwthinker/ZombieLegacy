#include "typedefs.h"
#include "building.h"
#include "linefeature.h"
#include "map.h"
#include <algorithm> 
#include <vector>
#include <cstdlib>
#include <ctime>

//			   0
//		______________
//		|			 |
//		|			 |    1
//	3   |			 |
//		|____________|
//				2
namespace zombie {

	float distPointToLine(LineFeature l, Position p);
	float distPointToPoint(Position p1, Position p2);

	class Tile {
	public:
		Tile() {
			tileSide_ = 10.0f;
			snapDist_ = 0.3f;
		}

		Tile(std::string filename) {

		}

		Tile(Map m) {
			buildings_ = m.getBuildings();
			roads_ = m.getRoads();
			tileSide_ = m.maxX() - m.minX();
			snapDist_ = 0.5f;
		}

		Tile(std::vector<BuildingPtr> buildings, std::vector<LineFeature> roads, std::string desc) {
			buildings_ = buildings;
			roads_ = roads;
			tileSide_ = 10.0f;
			snapDist_ = 0.3f;
			desc_ = desc;
		}

		std::vector<LineFeature> getRoads() {
			return roads_;
		}

		std::vector<BuildingPtr> getBuildings() {
			return buildings_;
		}

		float getTileSide() {
			return tileSide_;
		}

		// Pos referes to Tile t candidate position. i is index of the existing tile
		bool isCompatible(Tile t, int pos, int i) {

			// DEFINING LINE FOR  THE CANDIDATE/EXISTING INTERSECTION.  *************************************
			int relation; // 0,1,2,3
			LineFeature section;

			if (i > pos) {
				if (i == pos+1) {
					relation = 1; 
					section = LineFeature(Position(tileSide_,0),Position(tileSide_,tileSide_));
				} else {
					relation = 0;	
					section = LineFeature(Position(0,tileSide_),Position(tileSide_,tileSide_));					
				}
			} else if ( i < pos) {
				if (i == pos-1) {
					relation = 3;	
					section = LineFeature(Position(0,0),Position(0,tileSide_));					
				} else {
					relation = 2;	
					section = LineFeature(Position(0,0),Position(tileSide_,0));					
				}
			}

			// EXTRACTING CONNECTIONS ***************************************************************************
			std::vector<Position> candidateConnections;
			std::vector<Position> existingConnections;


			//roads.push_back(LineFeature(Position(0,50),Position(50,50)));
			//roads.push_back(LineFeature(Position(50,50),Position(50,0)));

			// CANDIDATE CONNECTIONS
			for (LineFeature l : t.getRoads()) {
				if (distPointToLine(section,l.getStart()) < snapDist_ ) {
					candidateConnections.push_back(l.getStart());
				}
				if (distPointToLine(section,l.getEnd()) < snapDist_ ) {
					candidateConnections.push_back(l.getEnd());
				}
			}

			// EXISTING CONNECTIONS - PERFORMING TRANSLATION ON TILE CONNECTION POSITIONS
			for (LineFeature l : roads_) {
				Position translatedStart;
				Position translatedEnd;
				switch(relation) {
				case 0 :
					translatedStart = Position(l.getStart().x,l.getStart().y+tileSide_);
					translatedEnd = Position(l.getEnd().x,l.getEnd().y+tileSide_);
					break;
				case 1 :
					translatedStart = Position(l.getStart().x+tileSide_,l.getStart().y);
					translatedEnd = Position(l.getEnd().x+tileSide_,l.getEnd().y);
					break;
				case 2 :
					translatedStart = Position(l.getStart().x,l.getStart().y-tileSide_);
					translatedEnd = Position(l.getEnd().x,l.getEnd().y-tileSide_);
					break;
				case 3 :
					translatedStart = Position(l.getStart().x-tileSide_,l.getStart().y);
					translatedEnd = Position(l.getEnd().x-tileSide_,l.getEnd().y);
					break;

				}

				if (distPointToLine(section,translatedStart) < snapDist_ ) {
					existingConnections.push_back(translatedStart);
				}
				if (distPointToLine(section,translatedEnd) < snapDist_ ) {
					existingConnections.push_back(translatedEnd);
				}
			}

			// TESTING CONNECTIONS - MATCHING AMOUNT? EACH CONNECTION HAS A CORRESPONDING MATCH IN THE OTHER TILE? **********************'
			if (candidateConnections.size() != existingConnections.size()){
				return false;
			} else {
				int nbrOfMatches = 0;
				for (Position c : candidateConnections) {
					for (Position e : existingConnections) {
						if (distPointToPoint(c,e) < snapDist_) {
							nbrOfMatches++; 
						}
					}
				}
				if(nbrOfMatches != candidateConnections.size()) {
					return false;
				}		
			}

			// ALL CONNECTIONS IS OK
			return true;
		}

		std::string desc_;

	private:
		float tileSide_;
		float snapDist_;
		std::vector<BuildingPtr> buildings_;
		std::vector<LineFeature> roads_;	
	};

	class TileManager {
	public:
		TileManager(int nbrOfTiles) {
			int nbrOfTiles_ = nbrOfTiles;
		}
		TileManager(const std::vector<Tile>& tiles, int nbrOfTiles) {
			nbrOfTiles_ = nbrOfTiles;
			unsortedTiles_ = tiles;
			srand((unsigned int)std::time(NULL));
		}

		bool sortTiles() {
			for (int i = 0; i < nbrOfTiles_*nbrOfTiles_; i++) {

				std::random_shuffle(unsortedTiles_.begin(),unsortedTiles_.end());
				for (unsigned int j = 0; j < unsortedTiles_.size(); j++) { 
					if(testTilePosition(unsortedTiles_[j],i)) {
						sortedTiles_.push_back(unsortedTiles_[j]);
						//sortedTiles_[i] = unsortedTiles_[j];
						break;
					}
				}				
			}
			return true;
		}

		Map stitchTiles() {
			std::vector<BuildingPtr> allBuildings;
			std::vector<LineFeature> allRoads;			

			for (int i = 0; i < nbrOfTiles_*nbrOfTiles_; i++) {
				std::vector<Position> corners;
				// Add buildings from tile
				for (BuildingPtr b : sortedTiles_[i].getBuildings()) {
					std::vector<Position> modifiedCorners;
					corners = b->getCorners();
					for (Position p : corners) {						
						p.x = p.x + sortedTiles_[i].getTileSide() * (i % nbrOfTiles_);
						p.y = p.y + sortedTiles_[i].getTileSide() * (i / nbrOfTiles_);
						modifiedCorners.push_back(p);
					}
					BuildingPtr building = BuildingPtr(new Building(modifiedCorners));
					allBuildings.push_back(building);
				}
				// Add roads from tile
				for (LineFeature l : sortedTiles_[i].getRoads()) {
					Position p1 = Position(l.getStart().x+ sortedTiles_[i].getTileSide() * (i % nbrOfTiles_),l.getStart().y+ sortedTiles_[i].getTileSide() * (i / nbrOfTiles_));
					Position p2 = Position(l.getEnd().x+ sortedTiles_[i].getTileSide() * (i % nbrOfTiles_),l.getEnd().y+ sortedTiles_[i].getTileSide() * (i / nbrOfTiles_));
					allRoads.push_back(LineFeature(p1,p2));
				}
			}

			//double t = sortedTiles_[0].getTileSide()*nbrOfTiles_ / 2.0;
			// Create map
			//Position center = Position(sortedTiles_[0].getTileSide()*nbrOfTiles_ / 2.0,sortedTiles_[0].getTileSide()*nbrOfTiles_ / 2.0);
			//double width = sortedTiles_[0].getTileSide()*nbrOfTiles_;
			//double height = sortedTiles_[0].getTileSide()*nbrOfTiles_;
			return Map(100*nbrOfTiles_,allBuildings,allRoads);
		}

		Tile getRandomTile() {
			int index = rand() % unsortedTiles_.size();
			return unsortedTiles_[index];
		}

		bool testTilePosition(Tile t, int pos) {
			std::vector<int> indexes =  getRelatedIndexes(pos);
			for (unsigned int i : indexes) {
				if(sortedTiles_.size() > i) { // if tile is placed test compability
					if(!sortedTiles_[i].isCompatible(t,pos,i)) {
						return false;
					}
				}
			}
			//std::cout << pos << ": " << t.desc_ << std::endl;
			return true;
		}

		std::vector<int> getRelatedIndexes(int pos) {
			std::vector<int> indexes;
			int p[4];
			p[0] = pos + nbrOfTiles_;
			p[1] = pos + 1;								//(1 - (pos % nbrOfTiles_ == nbrOfTiles_-1));
			if (pos % nbrOfTiles_ == nbrOfTiles_ - 1) {
				p[1] = pos;
			}
			p[2] = pos - nbrOfTiles_;
			p[3] = pos - 1;								//(1 - (pos % nbrOfTiles_ == 0))/2;
			if (pos % nbrOfTiles_ == 0) {
				p[3] = pos;
			}
			for (int i = 0; i < 4; i++) {
				if(p[i] >= 0 && p[i] < nbrOfTiles_*nbrOfTiles_ && p[i] != pos) {
					indexes.push_back(p[i]);
				}
			}
			return indexes;
		}

	private:
		int nbrOfTiles_;
		std::vector<Tile> sortedTiles_;
		std::vector<Tile> unsortedTiles_;
	};


	// END TILE / TILEMANAGER *********************************************************************************

	Map createTiledMap() {
		// Create variables
		BuildingPtr b;
		std::vector<Position> corners;
		std::vector<BuildingPtr> buildings;

		// TILE S ************************************************

		corners.push_back(Position(8.0,1.5));
		corners.push_back(Position(8.5,1.5));
		corners.push_back(Position(8.5,2.5));
		corners.push_back(Position(8.0,2.0));
		b = BuildingPtr(new Building(corners));

		buildings.push_back(b);
		std::vector<LineFeature> roads;
		Tile s = Tile(buildings,roads,"litetHus");

		// TILE T ************************************************
		corners.clear();

		corners.push_back(Position(1.5,4.5));
		corners.push_back(Position(5.5,4.5));
		corners.push_back(Position(6.5,1));
		corners.push_back(Position(3,1));
		b = BuildingPtr(new Building(corners));
		buildings.clear();
		buildings.push_back(b);
		roads.clear();
		Tile t = Tile(buildings,roads,"stortHus");

		// TILE U ************************************************
		buildings.clear();;
		roads.clear();
		roads.push_back(LineFeature(Position(50,0),Position(5.0,10.0)));
		roads.push_back(LineFeature(Position(0,50),Position(10.0,5.0)));
		Tile u = Tile(buildings,roads,"Korsning");

		// TILE V ************************************************
		buildings.clear();;
		roads.clear();
		roads.push_back(LineFeature(Position(0,5.0),Position(5.0,5.0)));
		roads.push_back(LineFeature(Position(5.0,5.0),Position(5.0,10.0)));
		Tile v = Tile(buildings,roads,"UppSveng");

		// TILE W ************************************************
		buildings.clear();;
		roads.clear();
		roads.push_back(LineFeature(Position(0,5.0),Position(5.0,5.0)));
		roads.push_back(LineFeature(Position(5.0,5.0),Position(5.0,0)));
		Tile w = Tile(buildings,roads,"NedSveng");

		// TILE AA ************************************************
		buildings.clear();;
		roads.clear();
		roads.push_back(LineFeature(Position(5.0,0),Position(5.0,10.0)));
		Tile AA = Tile(buildings,roads,"Raktupp");

		// TILE BB ************************************************
		buildings.clear();;
		roads.clear();
		roads.push_back(LineFeature(Position(0,5.0),Position(10.0,5.0)));
		Tile BB = Tile(buildings,roads,"Rakthorisont");

		// TILE V ************************************************
		buildings.clear();;
		roads.clear();
		roads.push_back(LineFeature(Position(0.5,0),Position(5,5.0)));
		roads.push_back(LineFeature(Position(5.0,5.0),Position(10,5.0)));
		Tile CC = Tile(buildings,roads,"LeftUppSveng");

		// TILE W ************************************************
		buildings.clear();;
		roads.clear();
		roads.push_back(LineFeature(Position(10,5),Position(5.0,5)));
		roads.push_back(LineFeature(Position(5.0,5.0),Position(5.0,10)));
		Tile DD = Tile(buildings,roads,"LeftNedSveng");

		std::vector<Tile> tiles;
		tiles.push_back(s);
		tiles.push_back(t);
		tiles.push_back(u);
		tiles.push_back(v);
		tiles.push_back(w);
		tiles.push_back(AA);
		tiles.push_back(BB);
		tiles.push_back(CC);
		tiles.push_back(DD);
		TileManager tManager(tiles,30);
		tManager.sortTiles();
		return tManager.stitchTiles();
	}

	Map createTiledMap(Map m) {
		std::vector<Tile> tiles;
		tiles.push_back(Tile(m));
		TileManager tManager(tiles,1);
		tManager.sortTiles();
		return tManager.stitchTiles();

	}

	float distPointToLine(LineFeature l, Position p) {
		// dist(line,point) = dist(ax+by+c=0,p) = abs(a*p.x+b*p.y+c)/sqrt(a^2+b^2) where
		// a = (y1 – y2) 
		// b = (x2 – x1)
		// c = (x1*y2 – x2*y1)

		double a = l.getStart().y - l.getEnd().y;
		double b = l.getEnd().x - l.getStart().x;
		double c = l.getStart().x * l.getEnd().y - l.getEnd().x * l.getStart().y;

		return abs(a*p.x+b*p.y+c) / sqrt(a*a+b*b);		 
	}

	float distPointToPoint(Position p1, Position p2) {
		return sqrt((p1.x-p2.x)*(p1.x-p2.x)+(p1.y-p2.y)*(p1.y-p2.y));
	}

}	// namespace zombie