#ifndef SHOT_H
#define SHOT_H

#include "task.h"
#include "typedefs.h"

namespace zombie {

	class Shot : public Task {
	public:
		Shot(Position start, Position end, float startTime);
		bool update(float time) override;
	
	private:
		float startTime_;
		Position start_;
		Position end_;
	};

} // Namespace zombie.

#endif // SHOT_H
