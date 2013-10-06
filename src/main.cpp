#include "zombiewindow.h"

#include <tinyxml2.h>

int main (int argc, char** argv) {
	tinyxml2::XMLDocument xmlDoc;
	xmlDoc.LoadFile("zombie.xml");

    zombie::ZombieWindow game;
	game.startLoop();

    return 0;
}
