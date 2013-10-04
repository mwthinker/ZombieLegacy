#ifndef MAPDRAW_H
#define MAPDRAW_H

#include "task.h"
#include "gamesprite.h"

namespace zombie {

	class Map;

	class MapDraw : public Task {
	public:
		MapDraw(const Map& map);
		bool update(double time) override;

	private:
		void draw();

		const Map& map_;
		mw::Sprite grass_;
	};

} // Namespace zombie.

#endif // MAPDRAW_H
