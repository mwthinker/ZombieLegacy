#include "zombiewindow.h"
#include "gamedataentry.h"

int main (int argc, char** argv) {

	bool skipMenu = false;
	if (argc > 1) {
		skipMenu = true;
	}

	// Load game data.
	zombie::GameDataEntry gameDataEntry("zombie.xml");

	zombie::ZombieWindow zombieWindow(gameDataEntry.getChildEntry("zombieGame"), skipMenu);
	zombieWindow.startLoop();

    return 0;
}
