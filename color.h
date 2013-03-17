#ifndef COLOR_H
#define COLOR_H

#include <SDL_opengl.h>

// Holds RGB-color needed to set the color in openGl.
class Color {
public:
	Color() {
		red_ = 1.0;
		green_ = 1.0;
		blue_ = 1.0;
		alpha_ = 1.0;
	}

	Color(double red, double green, double blue, double alpha = 1.0) {
		red_ = red;
		green_ = green;
		blue_ = blue;
		alpha_ = alpha;
	}

	void glColor3d() const {
		::glColor3d(red_,green_,blue_);
	}

	void glColor4d() const {
		::glColor4d(red_,green_,blue_,alpha_);
	}

	double red_, green_, blue_, alpha_;
};
// BLACK/WHITE
const Color BLACK(0,0,0);
const Color WHITE(1,1,1);

//COLORS
const Color RED(1,0,0);
const Color GREEN(0,1,0);
const Color BLUE(0,0,1);

// GRAYSCALES
const Color GRAY1(0.1,0.1,0.1);
const Color GRAY2(0.2,0.2,0.2);
const Color GRAY3(0.3,0.3,0.3);
const Color GRAY4(0.4,0.4,0.4);

#endif // COLOR_H