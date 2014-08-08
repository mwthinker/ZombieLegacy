#ifndef CONTACTLISTENER_H
#define CONTACTLISTENER_H

#include "box2ddef.h"
#include "gameinterface.h"

namespace zombie {

	class ContactListener : public b2ContactListener {
	public:
		ContactListener(GameInterface& gameInterface, float impulseThreshold);

		void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse) override;

		void BeginContact(b2Contact* contact) override;

		// Is called when the contact has get out of view or is destroyed.
		void EndContact(b2Contact* contact) override;

	private:
		float impulseThreshold_;
		GameInterface& gameInterface_;
	};

} // Namespace zombie.

#endif // CONTACTLISTENER_H

