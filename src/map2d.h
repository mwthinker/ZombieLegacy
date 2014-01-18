#ifndef MAP2D_H
#define MAP2D_H

#include <mw/sprite.h>

namespace zombie {

	class Map2D {
	public:
		Map2D();
		Map2D(float minX, float maxX, float minY, float maxY);
		
		void draw(float time);

	private:
		float minX_, maxX_, minY_, maxY_;
		mw::Sprite grass_;
	};

} // Namespace zombie.

#endif // MAP2D_H
