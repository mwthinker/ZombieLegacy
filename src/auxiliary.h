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

	inline void glTranslate2f(b2Vec2 vec) {
		::glTranslatef(vec.x, vec.y, 0.0);
	}
		
	inline void glScale2f(float scale) {
		::glScalef(scale, scale, 1.f);
	}

	inline void glRotatef(float angle) {
		::glRotatef(angle, 0, 0, 1);
	}

	// Returns a random number. The output is uniformly distributed on the interval [a, b).
	float random();

	int randomInt(int lower, int higher);

} // Namespace zombie.

#endif // AUXILIARY_H
