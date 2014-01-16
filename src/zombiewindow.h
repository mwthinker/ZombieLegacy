#ifndef ZOMBIEWINDOW_H
#define ZOMBIEWINDOW_H

#include "zombiegame.h"
#include "gamedata.h"
#include "gamefont.h"

#include <gui/borderlayout.h>
#include <gui/horizontallayout.h>
#include <gui/button.h>
#include <gui/frame.h>
#include <mw/sprite.h>
#include <SDL_opengl.h>

#include <iostream>

namespace zombie {

	class ZombieComponent : public gui::Component {
	public:
		ZombieComponent(ZombieGame& zombieGame) : zombieGame_(zombieGame) {
			setGrabFocus(true);
		}

		void draw(Uint32 deltaTime) {
			gui::Dimension dim = getSize();

			// Draw graphic.
			glPushMatrix();
			if (dim.width_ > dim.height_) {
				double dist = (dim.width_ - dim.height_) * 0.5;
				// Keep the view in centre.
				glTranslated(dist, 0, 0);
				// Scale correctly, keep proportions.
				glScaled(dim.height_, dim.height_, 1);
			} else {
				double dist = (dim.height_ - dim.width_) * 0.5;
				// Keep the view in centre.
				glTranslated(0, dist, 0);
				// Scale correctly, keep proportions.
				glScaled(dim.width_, dim.width_, 1);
			}
			zombieGame_.update(deltaTime / 1000.f);

			glPopMatrix();
		}

		void handleKeyboard(const SDL_Event& keyEvent) override {
			zombieGame_.eventUpdate(keyEvent);

			switch (keyEvent.type) {
				case SDL_KEYDOWN:
					switch (keyEvent.key.keysym.sym) {
						case SDLK_ESCAPE:
							break;
						case SDLK_PAGEUP:
							zombieGame_.zoom(1.1f);
							break;
						case SDLK_PAGEDOWN:
							zombieGame_.zoom(1 / 1.1f);
							break;
						case SDLK_RETURN:
							zombieGame_.startGame();
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

	private:
		ZombieGame& zombieGame_;
	};

	gui::Button* createButton(std::string str) {
		gui::Button* button = new gui::Button(str, font15);
		button->setAutoSizeToFitText(true);
		return button;
	}

	class ZombieWindow : public gui::Frame {
	public:
		ZombieWindow(const GameData& gameData) : gui::Frame(gameData.getWidth(), gameData.getHeight(), true, "Zombie", "images/icon.bmp"), zombieGame_(gameData) {
			add(new ZombieComponent(zombieGame_), gui::BorderLayout::CENTER);
			gui::Panel* panel = new gui::Panel;
			panel->setPreferredSize(100, 50);
			add(panel, gui::BorderLayout::SOUTH);
			panel->add(createButton("Button"));
			panel->add(createButton("Button"));
			panel->add(createButton("Button"));
			panel->add(createButton("Button"));

			setDefaultClosing(true);
		}

		~ZombieWindow() {
		}
	
	private:
		ZombieGame zombieGame_;
	};

} // Namespace zombie.

#endif // ZOMBIEWINDOW_H
