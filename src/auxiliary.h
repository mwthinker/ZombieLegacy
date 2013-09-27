#ifndef AUXILIARY_H
#define AUXILIARY_H

#include <mw/mathvector.h>

#include <cmath>

namespace zombie {

	static double calculateDifferenceBetweenAngles(double firstAngle, double secondAngle) {
		double difference = firstAngle - secondAngle;
		while (difference < -mw::PI) difference += 2*mw::PI;
		while (difference > mw::PI) difference -= 2*mw::PI;
		return difference;
	}

	static double calculateAnglePointToPoint(double x, double y, double pX, double pY) {
		return atan2(pY-y,pX-x);
	}

	static int intSign(double a) {
		return a < 0 ? -1 : 1;
	}

	// May not working?
	static bool isPointBetweenParallelLines(mw::MathVector linePosition, double angleDir, double lineWidth, mw::MathVector point) {
		mw::MathVector line = mw::MathVector(std::cos(angleDir),std::sin(angleDir));
		mw::MathVector dl = mw::MathVector(std::cos(mw::PI*0.5+angleDir),std::sin(mw::PI*0.5+angleDir))*lineWidth*0.5; // Turn 90 deg counter clockwise.
		return 0 == (intSign((line + dl).cross(linePosition-point).z_) - intSign((line + dl).cross(linePosition-point).z_));
	}

	// Returns a random number. The output is uniformly distributed on the interval [a, b).
	float random();

} // Namespace zombie.

#endif // AUXILIARY_H
