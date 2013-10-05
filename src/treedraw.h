#ifndef TREEDRAW_H
#define TREEDRAW_H

#include "task.h"
#include "gamesprite.h"

namespace zombie {

	class Map;

	class TreeDraw : public Task {
	public:
		TreeDraw(const Map& map);
		
		bool update(double time) override;

		const Map& map_;
		mw::Sprite tree_;
	};

} // Namespace zombie.

#endif // ROADDRAW_H
