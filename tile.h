#include "typedefs.h"
#include "linefeature.h"
#include <algorithm> 
#include <vector>

//			   1
//		______________
//		|			 |
//		|			 |    2
//	4   |			 |
//		|____________|
//				3

namespace zombie {

	class Tile {
		public:
			Tile() {
			
			}

		bool isCompatible(Tile t, int side) {			
			return true;
		}

		private:
		std::vector<Position> sides_;
		std::vector<Building> buildings_;
		std::vector<LineFeature> roads_;
	
	};

	class TileManager {
	public:
		TileManager(int nbrOfTiles) {
			int nbrOfTiles_ = nbrOfTiles;
		}
		TileManager(std::vector<Tile> tiles) {
			unsortedTiles_ = tiles;
		}

		bool sortTiles() {
			for (int i = 0; i < nbrOfTiles_; i++) {
				std::random_shuffle(unsortedTiles_.begin(),unsortedTiles_.end());
				for (int j = 0; j < unsortedTiles_.size(); j++) { 
					if(testTilePosition(unsortedTiles_[j],i)) {
						sortedTiles_[i] = unsortedTiles_[j];
						break;
					}
				}				
			}			
		}

		Tile getRandomTile() {
			int index = rand() % unsortedTiles_.size();
			return unsortedTiles_[index];
		}

		bool testTilePosition(Tile t, int pos) {
			return true;
		}

	private:
		int nbrOfTiles_;
		std::vector<Tile> sortedTiles_;
		std::vector<Tile> unsortedTiles_;
	};

}	// namespace zombie