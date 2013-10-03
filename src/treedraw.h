#ifndef TREEDRAW_H
#define TREEDRAW_H

#include "graphictask.h"
#include "gamesprite.h"

namespace zombie {

	class Map;

	class TreeDraw : public GraphicTask {
	public:
		TreeDraw(const Map& map);
		
		bool draw(double time) override;

		const Map& map_;
		mw::Sprite tree_;
	};

} // Namespace zombie.

#endif // ROADDRAW_H
