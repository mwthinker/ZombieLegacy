#include "zombiewindow.h"
#include "zombiegame.h"
#include "gamedata.h"

#include <gui/borderlayout.h>
#include <gui/horizontallayout.h>
#include <gui/button.h>
#include <gui/frame.h>
#include <gui/panel.h>
#include <gui/label.h>
#include <gui/verticallayout.h>

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
					}
			}
		}

		std::shared_ptr<gui::Button> createButton(std::string str, const mw::Font& font) {
			auto button = std::make_shared<gui::Button>(str, font);
			button->setAutoSizeToFitText(true);
			return button;
		}

	} // Anonymous namespace.

	ZombieWindow::ZombieWindow(GameData& gameData) : gui::Frame(gameData.getWindowWidth(), gameData.getWindowHeight(), true, "Zombie", "images/icon.bmp"), gameData_(gameData) {
		SDL_SetWindowPosition(mw::Window::getSdlWindow(), gameData.getWindowXPosition(), gameData.getWindowYPosition());
		SDL_SetWindowMinimumSize(mw::Window::getSdlWindow(), 400, 400);
		if (gameData.isWindowMaximized()) {
			SDL_MaximizeWindow(mw::Window::getSdlWindow());
		}

		addWindowListener([&](gui::Frame& frame, const SDL_Event& sdlEvent) {
			switch (sdlEvent.type) {
				case SDL_WINDOWEVENT:
					switch (sdlEvent.window.event) {
						case SDL_WINDOWEVENT_RESIZED:
							if (!(SDL_GetWindowFlags(mw::Window::getSdlWindow()) & SDL_WINDOW_MAXIMIZED)) {
								// The Window's is not maximized. Save size!
								gameData.setWindowSize(sdlEvent.window.data1, sdlEvent.window.data2);
							}
							break;
						case SDL_WINDOWEVENT_MOVED:
							if (!(SDL_GetWindowFlags(mw::Window::getSdlWindow()) & SDL_WINDOW_MAXIMIZED)) {
								// The Window's is not maximized. Save position!
								int x, y;
								SDL_GetWindowPosition(mw::Window::getSdlWindow(), &x, &y);
								gameData.setWindowPosition(x, y);
							}
							break;
						case SDL_WINDOWEVENT_MAXIMIZED:
							gameData.setWindowMaximized(true);
							break;
						case SDL_WINDOWEVENT_RESTORED:
							gameData.setWindowMaximized(false);
							break;
					}
					break;
			}
		});

		zombieGame_ = std::make_shared<ZombieGame>(gameData);
		// Always react on key events!
		zombieGame_->setGrabFocus(true);

		menuFrameIndex_ = getCurrentPanelIndex();
		playFrameIndex_ = addPanelBack();
		customGameFrameIndex_ = addPanelBack();
		highscoreFrameIndex_ = addPanelBack();

		initMenuFrame();
		initplayFrame();
		initCustomplayFrame();
		initHighscoreFrame();
		setCurrentPanel(menuFrameIndex_);

		setDefaultClosing(true);

		zombieGame_->addKeyListener(std::bind(&handleKeyboard, std::placeholders::_1, std::placeholders::_2));
	}

	void ZombieWindow::initMenuFrame() {
		setCurrentPanel(menuFrameIndex_);
		auto panel = std::make_shared<gui::Panel>();
		add(panel, gui::BorderLayout::CENTER);
		panel->setLayout(std::make_shared<gui::VerticalLayout>());
		panel->setBackground(mw::Sprite("images/menu.jpg"));
		mw::Font font = gameData_.getDefaultFont(20);

		auto text = std::make_shared<gui::Label>("Zombie", gameData_.getDefaultFont(60));
		text->setTextColor(1, 1, 1);
		panel->addToGroup(text);

		auto play = createButton("Play", font);
		play->addActionListener([&](gui::Component&) {
			setCurrentPanel(playFrameIndex_);
			//gameData_.getMenuChoiceSound().play();
		});
		panel->addToGroup(play);

		auto customGame = createButton("Custom game", font);
		customGame->addActionListener([&](gui::Component&) {
			//gameData_.getMenuChoiceSound().play();
		});
		panel->addToGroup(customGame);

		auto highScore = createButton("Highscore", font);
		highScore->addActionListener([&](gui::Component&) {
			//gameData_.getMenuChoiceSound().play();
		});
		panel->addToGroup(highScore);

		auto quit = createButton("Quit", font);
		quit->addActionListener([&](gui::Component&) {
			//gameData_.getMenuChoiceSound().play();
			Window::quit();
		});
		panel->addToGroup(quit);
	}

	void ZombieWindow::initplayFrame() {
		setCurrentPanel(playFrameIndex_);

		add(zombieGame_, gui::BorderLayout::CENTER);
		auto panel = std::make_shared<gui::Panel>();
		panel->setPreferredSize(100, 50);
		add(panel, gui::BorderLayout::SOUTH);
		panel->setBackgroundColor(0, 0, 0);
		mw::Font font = gameData_.getDefaultFont(15);

		panel->add(createButton("Button", font));
		panel->add(createButton("Button", font));
		panel->add(createButton("Button", font));
		panel->add(createButton("Button", font));
	}

	void ZombieWindow::initCustomplayFrame() {
		setCurrentPanel(customGameFrameIndex_);
	}

	void ZombieWindow::initHighscoreFrame() {
		setCurrentPanel(highscoreFrameIndex_);
	}

} // Namespace zombie.
