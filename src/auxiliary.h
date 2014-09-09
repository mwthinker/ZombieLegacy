#ifndef AUXILIARY_H
#define AUXILIARY_H

#include "box2ddef.h"

#include <mw/opengl.h>

#include <cmath>

namespace zombie {

	static double calculateDifferenceBetweenAngles(double firstAngle, double secondAngle) {
		double difference = firstAngle - secondAngle;
		while (difference < -PI) difference += 2*PI;
		while (difference > PI) difference -= 2*PI;
		return difference;
	}

	static float calculateAnglePointToPoint(b2Vec2 p1, b2Vec2 p2) {
		return atan2(p2.y - p1.y, p2.x - p1.x);
	}

	static double calculateAnglePointToPoint(float x, float y, float pX, float pY) {
		return atan2(pY-y,pX-x);
	}	

	static int intSign(double a) {
		return a < 0 ? -1 : 1;
	}	

	static Position directionVector(float angle) {
		return Position(std::cos(angle), std::sin(angle));
	}

	// Returns a random number. The output is uniformly distributed on the interval [a, b).
	float random();

	float random(float min, float max);

	int randomInt(int lower, int higher);

} // Namespace zombie.

#endif // AUXILIARY_H
