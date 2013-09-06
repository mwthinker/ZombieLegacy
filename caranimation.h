#ifndef CARANIMATION_H
#define CARANIMATION_H

#include "graphictask.h"

#include <mw/sprite.h>

namespace zombie {

	class Car;

	class CarAnimation : public GraphicTask {
	public:
		CarAnimation(Car* car);
		~CarAnimation();

		void draw(double time) override;
		bool isRunning() const override;
	
	private:				
		int idCarObject_;
		mw::Sprite carSprite_;
		double lastTime_;
	};

} // Namespace zombie.

#endif // CARANIMATION_H
