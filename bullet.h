#ifndef BULLET_H
#define BULLET_H

#include <mw/mathvector.h>

struct Bullet {
public:
	mw::MathVector postion_;
	double damage_;
	double direction_;
	double range_;	
};

#endif // BULLET_H