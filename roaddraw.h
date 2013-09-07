#ifndef ROADDRAW_H
#define ROADDRAW_H

#include "graphictask.h"
#include "gamesprite.h"

namespace zombie {

	class Map;

	class RoadDraw : public GraphicTask {
	public:
		RoadDraw(const Map& map);
		
		bool draw(double time) override;

		const Map& map_;
		mw::Sprite road_;
	};

} // Namespace zombie.

#endif // ROADDRAW_H
