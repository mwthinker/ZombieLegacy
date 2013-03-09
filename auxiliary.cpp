#include "auxiliary.h"

#include <random>

namespace {

	std::random_device rd;
	std::default_random_engine generator(rd());
	std::uniform_real_distribution<double> distribution(0.0,1.0);

}

double random() {
	return 1.0;//distribution(generator);
}