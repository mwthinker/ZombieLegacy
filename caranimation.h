#ifndef CARANIMATION_H
#define CARANIMATION_H

#include "typedefs.h"
#include "graphicobject.h"

#include <mw/sprite.h>

namespace zombie {

	class Car;

	class CarAnimation : public GraphicObject {
	public:
		CarAnimation(Car* carParam);
		~CarAnimation();

		void draw(float timestep);
	private:				
		
		mw::Sprite s_;
		Car* car_;
		double lastTime_;
		float time_;
	};

} // Namespace zombie.

#endif // CARANIMATION_H
