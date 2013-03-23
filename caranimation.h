#ifndef CARANIMATION_H
#define CARANIMATION_H

#include "typedefs.h"
#include "graphictask.h"

namespace zombie {

	class Car;

	class CarAnimation : public GraphicTask {
	public:
		CarAnimation(Car* carParam);
		~CarAnimation();
		
		void drawSecond(double time) override;

		bool isRunning() const override;
	private:
		void draw(double timestep);		
		
		mw::Sprite s_;
		Car* car_;
		double lastTime_;
	};

} // Namespace zombie.

#endif // CARANIMATION_H