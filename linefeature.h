#ifndef LINEFEATURE_H
#define LINEFEATURE_H

#include "typedefs.h"

namespace zombie {

	class LineFeature {
	public:
		LineFeature() {
		}

		LineFeature(Position p1, Position p2) : p1_(p1), p2_(p2) {
		}

		Position getStart() const {
			return p1_;
		}

		Position getEnd() const {
			return p2_;
		}

	private:
		Position p1_, p2_;	
	};

} // Namespace zombie.

#endif // LINEFEATURE_H
