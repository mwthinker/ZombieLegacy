#include "zombiewindow.h"

#include <json/json.h>

int main (int argc, char** argv) {

	bool skipMenu = false;
	if (argc > 1) {
		skipMenu = true;
	}

	zombie::ZombieWindow zombieWindow(skipMenu);
	zombieWindow.startLoop();

    return 0;
}
