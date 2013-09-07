#ifndef ZOMBIEGAME_H
#define ZOMBIEGAME_H

#include "zombieengine.h"
#include "device.h"
#include "map.h"

#include <SDL.h>

namespace zombie {

	class ZombieGame {
	public:
		ZombieGame(int width = 500, int height = 500);
		~ZombieGame();

		// Starts the game.
		void startGame();

		// Draws the graphic and (deltaTime) should be the time past 
		// from the previous call to this funtion.
		void update(float deltaTime);

		// Makes the game reacting on an event (windowEvent).
		void eventUpdate(const SDL_Event& windowEvent);

		void zoom(double scale);

		void updateSize(int width, int height);

	private:
		Map map_;
		ZombieEngine engine_;
		DevicePtr keyboard1_, keyboard2_;
	};

} // Namespace zombie_;

#endif // ZOMBIEGAME_H
