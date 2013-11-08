#ifndef GAMEINTERFACE_H
#define GAMEINTERFACE_H

namespace zombie {

	class GameInterface {
	public:
		// The current position of the human.
		virtual void humanPosition(float x, float y) {
		}

		// The human just died.
		virtual void humanDied() {
		}

		// A unit just died.
		virtual void unitDied(float x, float y, bool infected) {
		}

		// Unit is removed if the function returns true.
		virtual bool isUnitOutside(float x, float y, bool infected) {
			return false;
		}
	};

} // Namespace zombie.

#endif // GAMEINTERFACE_H
