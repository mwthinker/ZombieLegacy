#ifndef MAPDRAW_H
#define MAPDRAW_H

#include "graphictask.h"
#include "gamesprite.h"

namespace zombie {

	class Map;

	class MapDraw : public GraphicTask {
	public:
		MapDraw(const Map& map);
		bool draw(double time) override;

	private:
		void draw();

		const Map& map_;
		mw::Sprite grass_;
	};

} // Namespace zombie.

#endif // MAPDRAW_H
