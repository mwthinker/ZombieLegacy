#ifndef AIBEHAVIOR_H
#define AIBEHAVIOR_H

#include "input.h"

namespace zombie {

	class Unit;

	class AiBehavior {
	public:
		virtual ~AiBehavior() {
		}

		virtual Input calculateInput(const Unit* unit, double time) = 0;
	};

} // Namespace zombie.

#endif // AIBEHAVIOR_H
