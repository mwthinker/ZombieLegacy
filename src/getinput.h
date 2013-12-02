#ifndef GETINPUT_H
#define GETINPUT_H

#include "input.h"

#include <memory>

namespace zombie {

	class GetInput;
	typedef std::shared_ptr<GetInput> GetInputPtr;

	class GetInput {
	public:
		virtual ~GetInput() {
		}

		virtual Input getInput() const = 0;
	};

} // Namespace zombie.

#endif // GETINPUT_H
