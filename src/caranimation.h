#ifndef CARANIMAITON_H
#define CARANIMAITON_H

#include "task.h"
#include "car.h"

#include <mw/sprite.h>

namespace zombie {

	class CarAnimation : public Task {
	public:
		CarAnimation(State state, float width, float length, mw::Sprite sprite);
		
		// Updates the data describing the car.
		void updateData(Car* car, Car::CarEvent unitEvent);

		// Draws the car.
		bool update(float time) override;

	private:
		void eventHandler(Car::CarEvent carEvent);

		State state_;
		float width_, length_;

		mw::Sprite animation_;
		bool remove_;
	};

} // Namespace zombie.

#endif // CARANIMAITON_H
