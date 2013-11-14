#ifndef BLOOD_H
#define BLOOD_H

#include "typedefs.h"
#include "task.h"

#include <mw/sprite.h>

namespace zombie {

	class Blood : public Task {
	public:
		Blood(Position position, float currentTime);
		
		bool update(float time) override;

	private:
		float startTime_;
		Position position_;				
		float duration_;

		mw::Sprite blood_;
	};	

} // Namespace zombie.

#endif // BLOOD_H
