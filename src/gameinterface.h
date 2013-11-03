#ifndef GAMEINTERFACE_H
#define GAMEINTERFACE_H

namespace zombie {

	class GameInterface {
	public:
		virtual void humanPosition(float x, float y) {
		}

		virtual void humanDies() {
		}

		virtual void unitDie(float x, float y, bool infected) {
		}

		// Unit is removed if the function returns true.
		virtual bool isUnitOutside(float x, float y, bool infected) {
			return false;
		}
	};

} // Namespace zombie.

#endif // GAMEINTERFACE_H
