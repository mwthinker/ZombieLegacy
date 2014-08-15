#ifndef INVIEWQUERYCALLBACK_H
#define INVIEWQUERYCALLBACK_H

#include "box2ddef.h"

#include <vector>

namespace zombie {

	class InViewQueryCallback : public b2QueryCallback {
	public:
		std::vector<b2Fixture*> foundFixtures;

		bool ReportFixture(b2Fixture* fixture) {
			foundFixtures.push_back(fixture);
			return true; // Keep going to find all fixtures in the query area.
		}

		void reset() {
			foundFixtures.clear();
		}
	};

} // Namespace zombie.

#endif // INVIEWQUERYCALLBACK_H
