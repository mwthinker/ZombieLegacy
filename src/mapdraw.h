#ifndef MAPDRAW_H
#define MAPDRAW_H

#include "task.h"

#include <mw/sprite.h>

namespace zombie {

	class MapDraw : public Task {
	public:
		MapDraw(float minX, float maxX, float minY, float maxY);
		bool update(double time) override;

	private:
		void draw();

		float minX_, maxX_, minY_, maxY_;
		mw::Sprite grass_;
	};

} // Namespace zombie.

#endif // MAPDRAW_H
