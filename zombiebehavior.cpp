#include "zombiebehavior.h"
#include "building.h"

#include <vector>
#include <random>

#include <mw/mathvector.h>

typedef mw::MathVector Position;

namespace zombie {

	//std::default_random_engine generator_;

	std::uniform_real_distribution<double> ZombieBehavior::distribution_(0.0,1.0);
} // namespace zombie.

