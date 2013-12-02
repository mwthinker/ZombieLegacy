#ifndef AIBEHAVIOR_H
#define AIBEHAVIOR_H

#include "input.h"
#include "getinput.h"

namespace zombie {

	class Unit;

	class AiBehavior : public GetInput {
	public:
		virtual ~AiBehavior() {
		}

		virtual void calculateInput(const Unit* unit, double time) = 0;
	};

} // Namespace zombie.

#endif // AIBEHAVIOR_H
