#ifndef SHOT_H
#define SHOT_H

#include "graphictask.h"
#include "typedefs.h"

namespace zombie {

	class Shot : public GraphicTask {
	public:
		Shot(Position start, Position end, float startTime);
		bool draw(double time) override;
	
	private:
		float startTime_;
		Position start_;
		Position end_;
	};

} // Namespace zombie.

#endif // SHOT_H
