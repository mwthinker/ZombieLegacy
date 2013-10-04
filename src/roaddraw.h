#ifndef ROADDRAW_H
#define ROADDRAW_H

#include "task.h"
#include "gamesprite.h"

namespace zombie {

	class Map;

	class RoadDraw : public Task {
	public:
		RoadDraw(const Map& map);
		
		bool update(double time) override;

		const Map& map_;
		mw::Sprite road_;
	};

} // Namespace zombie.

#endif // ROADDRAW_H
