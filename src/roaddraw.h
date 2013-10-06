#ifndef ROADDRAW_H
#define ROADDRAW_H

#include "task.h"
#include "gamesprite.h"

namespace zombie {

	class RoadDraw : public Task {
	public:
		RoadDraw();
		
		bool update(double time) override;
		
		mw::Sprite road_;
	};

} // Namespace zombie.

#endif // ROADDRAW_H
