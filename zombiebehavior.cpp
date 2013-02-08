#include "zombiebehavior.h"

#include <vector>
#include <random>

namespace zombie {

	//std::default_random_engine generator_;

	std::uniform_real_distribution<double> ZombieBehavior::distribution_(0.0,1.0);
} // namespace zombie.

