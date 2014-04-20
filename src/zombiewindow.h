#ifndef ZOMBIEWINDOW_H
#define ZOMBIEWINDOW_H

#include <gui/frame.h>

#include <memory>

namespace zombie {

	class GameData;
	class ZombieGame;

	class ZombieWindow : public gui::Frame {
	public:
		ZombieWindow(GameData& gameData);

		~ZombieWindow() {
		}
	
	private:
		std::shared_ptr<ZombieGame> zombieGame_;
	};

} // Namespace zombie.

#endif // ZOMBIEWINDOW_H
