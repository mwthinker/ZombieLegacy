#include "auxiliary.h"

#include <random>

namespace zombie {

	namespace {

		std::random_device rd;
		std::default_random_engine generator(rd());
		std::uniform_real_distribution<float> distribution(0.0,1.0);

	}

	float random() {
		return distribution(generator);
	}

	int randomInt(int lower, int higher) {
		std::uniform_int_distribution<int> distribution(lower, higher);
		return distribution(generator);
	}

} // Namespace zombie.
