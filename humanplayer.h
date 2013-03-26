#ifndef HUMANPLAYER_H
#define HUMANPLAYER_H

#include "player.h"

#include <mw/signals/connection.h>
#include <SDL.h>

namespace zombie {

	class HumanPlayer : public Player {
	public:
		virtual ~HumanPlayer() {
			connection_.disconnect();
		}

		virtual void eventUpdate(const SDL_Event& windowEvent) = 0;

		void calculateInput(Unit* unit, double time) {
		}

		void updateUnitsInView(const std::vector<Unit*>& units) {
		}

		void setConnection(mw::signals::Connection connection) {
			connection_ = connection;
		}

	private:
		mw::signals::Connection connection_;
	};

} // namespace zombie.

#endif // HUMANPLAYER_H