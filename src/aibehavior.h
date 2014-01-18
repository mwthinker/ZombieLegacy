#ifndef AIBEHAVIOR_H
#define AIBEHAVIOR_H

#include "input.h"
#include "getinput.h"

#include <memory>

namespace zombie {

	class Unit;

	// Smart pointers.
	class AiBehavior;
	typedef std::shared_ptr<AiBehavior> AiBehaviorPtr;

	class AiBehavior : public GetInput {
	public:
		virtual ~AiBehavior() {
		}

		virtual void calculateInput(const Unit* unit, double time) = 0;
	};

} // Namespace zombie.

#endif // AIBEHAVIOR_H
