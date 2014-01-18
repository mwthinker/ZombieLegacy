#ifndef BLOOD_H
#define BLOOD_H

#include "typedefs.h"

#include <mw/sprite.h>

namespace zombie {

	class Blood {
	public:
		Blood(Position position, float currentTime);
		
		bool update(float time);

	private:
		float startTime_;
		Position position_;				
		float duration_;

		mw::Sprite blood_;
	};	

} // Namespace zombie.

#endif // BLOOD_H
