#include "treedraw.h"
#include "map.h"

#include <SDL_opengl.h>

namespace zombie {

	TreeDraw::TreeDraw(const Map& map) : map_(map) {
		tree_ = treeSprite;
	}

	bool TreeDraw::update(double time) {
		
		return true;
	}

} // Namespace zombie.
