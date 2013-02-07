#ifndef HUMANPLAYER_H
#define HUMANPLAYER_H

#include "player.h"

#include <SDL.h>

namespace zombie {

	class HumanPlayer : public Player {
	public:
		virtual ~HumanPlayer() {
		}

		virtual void eventUpdate(const SDL_Event& windowEvent) = 0;
	};

} // namespace zombie.

#endif // HUMANPLAYER_H