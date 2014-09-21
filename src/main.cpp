#include "zombiewindow.h"
#include "gamedataentry.h"

int main (int argc, char** argv) {

	// Load game data.
	zombie::GameDataEntry gameDataEntry("zombie.xml");

	zombie::ZombieWindow zombieWindow(gameDataEntry.getChildEntry("zombieGame"));
	zombieWindow.startLoop();

    return 0;
}
