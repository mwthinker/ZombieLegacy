#ifndef CARANIMATION_H
#define CARANIMATION_H

#include "task.h"

#include <mw/sprite.h>

namespace zombie {

	class Car;

	class CarAnimation : public Task {
	public:
		CarAnimation(Car* car);
		~CarAnimation();

		bool update(double time) override;
	
	private:				
		int idCarObject_;
		mw::Sprite carSprite_;
		double lastTime_;
	};

} // Namespace zombie.

#endif // CARANIMATION_H
