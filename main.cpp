#include "zombiewindow.h"

int main (int argc, char** argv) {
    zombie::ZombieWindow game;	

	switch (argc) {
	case 1:
		game.startLocal();
		break;
	case 2:
		game.startServer();
	case 3:
		game.startClient();
	}

	game.startLoop();

    return 0;
}