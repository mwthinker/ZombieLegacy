#ifndef SHOT_H
#define SHOT_H

#include "graphictask.h"
#include "typedefs.h"

namespace zombie {

	class Shot : public GraphicTask {
	public:
		Shot(Position start, Position end, float startTime);
		void draw(double time) override;		

		bool isRunning() const override;
	private:
		float startTime_;
		Position start_;
		Position end_;
		bool running_;
	};

} // Namespace zombie.

#endif // SHOT_H
