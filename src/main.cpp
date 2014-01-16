#include "zombiewindow.h"
#include "gamedata.h"

int main (int argc, char** argv) {

	// Load game data.
	zombie::GameData gameData("zombie.xml");

	zombie::ZombieWindow zombieWindow(gameData);
	zombie::ZombieWindow::startLoop();

    return 0;
}
