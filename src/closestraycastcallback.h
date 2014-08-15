#ifndef CLOSESTRAYCASTCALLBACK_H
#define CLOSESTRAYCASTCALLBACK_H

#include "box2ddef.h"

#include <functional>

namespace zombie {

	class ClosestRayCastCallback : public b2RayCastCallback {
	public:
		ClosestRayCastCallback(std::function<bool(b2Fixture*)> conditionFunc) {
			closestFraction_ = 1.f;
			fixture_ = nullptr;
			conditionFunc_ = conditionFunc;
		}

		b2Fixture* getFixture() const {
			return fixture_;
		}

		// Ray-cast callback.
		float32 ReportFixture(b2Fixture* fixture, const b2Vec2 &point, const b2Vec2 &normal, float32 fraction) override {
			// Is a physical fixture?
			if (conditionFunc_(fixture)) {
				fixture_ = fixture;
				closestFraction_ = fraction;
			}

			// Fraction to clip the ray for closest hit.
			return fraction;
		}

		float getFraction() const {
			return closestFraction_;
		}

		void reset() {
			closestFraction_ = 1.f;
			fixture_ = nullptr;
		}

	private:
		b2Fixture* fixture_;
		std::function<bool(b2Fixture*)> conditionFunc_;
		float closestFraction_;
	};

} // Namespace zombie.

#endif // CLOSESTRAYCASTCALLBACK_H

