#ifndef AUXILIARY_H
#define AUXILIARY_H

#include "box2ddef.h"

#include <SDL_opengl.h>

#include <cmath>

namespace zombie {

	static double calculateDifferenceBetweenAngles(double firstAngle, double secondAngle) {
		double difference = firstAngle - secondAngle;
		while (difference < -PI) difference += 2*PI;
		while (difference > PI) difference -= 2*PI;
		return difference;
	}

	static double calculateAnglePointToPoint(double x, double y, double pX, double pY) {
		return atan2(pY-y,pX-x);
	}

	static int intSign(double a) {
		return a < 0 ? -1 : 1;
	}

	inline void glTranslate2f(b2Vec2 vec) {
		glTranslatef(vec.x, vec.y, 0.0);
	}
		
	inline void glScale2f(float scale) {
		glScalef(scale, scale, 1.f);
	}

	// Returns a random number. The output is uniformly distributed on the interval [a, b).
	float random();

} // Namespace zombie.

#endif // AUXILIARY_H
