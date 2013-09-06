#ifndef AIBEHAVIOR_H
#define AIBEHAVIOR_H

#include "typedefs.h"

#include "input.h"
#include <vector>

namespace zombie {

	class Unit;

	class AiBehavior {
	public:
		AiBehavior();
		virtual ~AiBehavior();

		virtual Input calculateInput(const Unit* unit, double time) = 0;
	private:

	};

} // namespace zombie.

#endif // AIBEHAVIOR_H
