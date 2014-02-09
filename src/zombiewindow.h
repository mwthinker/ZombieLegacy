#ifndef ZOMBIEWINDOW_H
#define ZOMBIEWINDOW_H

#include "zombiegame.h"
#include "gamedata.h"
#include "gamefont.h"

#include <mw/sprite.h>

#include <gui/borderlayout.h>
#include <gui/horizontallayout.h>
#include <gui/button.h>
#include <gui/frame.h>

#include <SDL_opengl.h>

#include <iostream>

namespace zombie {

	namespace {

		void handleKeyboard(gui::Component& c, const SDL_Event& keyEvent) {
			ZombieGame& zombieGame = static_cast<ZombieGame&>(c);
			
			switch (keyEvent.type) {
				case SDL_KEYDOWN:
					switch (keyEvent.key.keysym.sym) {
						case SDLK_ESCAPE:
							break;
						case SDLK_PAGEUP:
							zombieGame.zoom(1.1f);
							break;
						case SDLK_PAGEDOWN:
							zombieGame.zoom(1 / 1.1f);
							break;
						case SDLK_RETURN:
							zombieGame.startGame();
							break;
						case SDLK_p:
							// Fall through.
						case SDLK_PAUSE:
							break;
						default:
							break;
					}
					break;
				default:
					break;
			}
		}

	} // Anonymous namespace.

	std::shared_ptr<gui::Button> createButton(std::string str) {
		auto button = std::make_shared<gui::Button>(str, font15);
		button->setAutoSizeToFitText(true);
		return button;
	}

	class ZombieWindow : public gui::Frame {
	public:
		ZombieWindow(const GameData& gameData) : gui::Frame(gameData.getWidth(), gameData.getHeight(), true, "Zombie", "images/icon.bmp") {
			zombieGame_ = std::make_shared<ZombieGame>(gameData);
			// Always react on key events!
			zombieGame_->setGrabFocus(true);
			add(zombieGame_, gui::BorderLayout::CENTER);
			auto panel = std::make_shared<gui::Panel>();
			panel->setPreferredSize(100, 50);
			add(panel, gui::BorderLayout::SOUTH);
			panel->add(createButton("Button"));
			panel->add(createButton("Button"));
			panel->add(createButton("Button"));
			panel->add(createButton("Button"));

			setDefaultClosing(true);

			zombieGame_->addKeyListener(std::bind(&handleKeyboard, std::placeholders::_1, std::placeholders::_2));
		}

		~ZombieWindow() {
		}
	
	private:
		std::shared_ptr<ZombieGame> zombieGame_;
	};

} // Namespace zombie.

#endif // ZOMBIEWINDOW_H
