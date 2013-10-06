#ifndef TREEDRAW_H
#define TREEDRAW_H

#include "task.h"
#include "gamesprite.h"

namespace zombie {

	class TreeDraw : public Task {
	public:
		TreeDraw();
		
		bool update(double time) override;

		mw::Sprite tree_;
	};

} // Namespace zombie.

#endif // ROADDRAW_H
