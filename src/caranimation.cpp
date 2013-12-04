#include "typedefs.h"
#include "auxiliary.h"
#include "car.h"
#include "caranimation.h"

#include <mw/sprite.h>

namespace zombie {

	CarAnimation::CarAnimation(State state, float width, float length, mw::Sprite sprite) : animation_(sprite) {
		remove_ = false;
		state_ = state;
		width_ = width;
		length_ = length;
	}	

	void CarAnimation::updateData(Car* car, Car::CarEvent unitEvent) {
		state_ = car->getState();
		width_ = car->getWidth();
		length_ = car->getLength();

		eventHandler(unitEvent);
	}

	bool CarAnimation::update(float time) {
		// Draw body
		glPushMatrix();
		glTranslate2f(state_.position_);
		glScaled(length_, width_, 1);
		glRotated(state_.angle_ * 180 / PI, 0, 0, 1);
		glRotated(270, 0, 0, 1);
		animation_.draw();
		glPopMatrix();

		return !remove_;
	}

	void CarAnimation::eventHandler(Car::CarEvent carEvent) {
		switch (carEvent) {
			case Car::ACTION:
				// Fall through!
				break;
			case Car::MOVED:
				// Fall through!
				break;
			case Car::REMOVED:
				remove_ = true;
				break;
		}
	}

} // Namespace zombie.
