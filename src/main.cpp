#include "zombiewindow.h"
#include "zombieentry.h"

int main (int argc, char** argv) {

	bool skipMenu = false;
	if (argc > 1) {
		skipMenu = true;
	}

	// Load game data.
	zombie::ZombieEntry zombieEntry("zombie.xml");

	zombie::ZombieWindow zombieWindow(zombieEntry.getChildEntry("zombieGame"), skipMenu);
	zombieWindow.startLoop();

    return 0;
}
