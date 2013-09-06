#ifndef PLAYER_H
#define PLAYER_H

#include "input.h"
#include "player.h"
#include "unit.h"

#include <vector>

namespace zombie {

	class Player {
	public:
		Player(const MovingObject* mOb) : id_(mOb->getId()) {
		}

		virtual ~Player() {
		}

		virtual void calculateInput(double time) {
		}

		virtual Input currentInput() = 0;

		inline int getId() const {
			return id_;
		}
		
	private:
		int id_;
	};

} // Namespace zombie.

#endif // PLAYER_H
