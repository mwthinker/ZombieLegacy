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
							break;
						case SDLK_p:
							// Fall through.
						case SDLK_PAUSE:
							break;
					}
			}
		}

		class Button : public gui::Button {
		public:
			Button(std::string text, mw::Font font) : gui::Button(text, font) {
				setFocusColor(0.8f, 0, 0, 0.5);
				setHoverColor(0.7f, 0, 0, 0.5);
				setPushColor(0.9f, 0, 0, 0.5);
				setBackgroundColor(1, 1, 1, 1);
				setAutoSizeToFitText(true);
			}
		};

		class TransparentPanel : public gui::Panel {
		public:
			TransparentPanel(float preferredWidth = 100, float preferredHeight = 100) {
				setBackgroundColor(1, 1, 1, 0);
				setPreferredSize(preferredWidth, preferredHeight);
			}

			virtual ~TransparentPanel() = default;

		};

	} // Anonymous namespace.

	ZombieWindow::ZombieWindow(bool skipMenu) : skipMenu_(skipMenu) {
		mw::Window::setPosition(GameData::getInstance().getWindowPositionX(), GameData::getInstance().getWindowPositionY());
		mw::Window::setWindowSize(GameData::getInstance().getWindowWidth(), GameData::getInstance().getWindowHeight());
		mw::Window::setIcon(GameData::getInstance().getWindowIcon());
		mw::Window::setResizeable(GameData::getInstance().isWindowResizable());
		mw::Window::setFullScreen(GameData::getInstance().isWindowMaximized());
	}

	ZombieWindow::~ZombieWindow() {
		//zombieEntry_.save();
	}

	void ZombieWindow::initOpenGl() {
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 16);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
		Window::setOpenGlVersion(2, 1);
		Frame::initOpenGl();
	}

	void ZombieWindow::initPreLoop() {
		Frame::initPreLoop();

		SDL_SetWindowMinimumSize(mw::Window::getSdlWindow(), 400, 400);

		addWindowListener([&](gui::Frame& frame, const SDL_Event& sdlEvent) {
			switch (sdlEvent.type) {
				case SDL_WINDOWEVENT:
					switch (sdlEvent.window.event) {
						case SDL_WINDOWEVENT_RESIZED:
							if (!(SDL_GetWindowFlags(mw::Window::getSdlWindow()) & SDL_WINDOW_MAXIMIZED)) {
								// The Window's is not maximized. Save size!
								GameData::getInstance().setWindowWidth(sdlEvent.window.data1);
								GameData::getInstance().setWindowHeight(sdlEvent.window.data2);
							}
							break;
						case SDL_WINDOWEVENT_MOVED:
							if (!(SDL_GetWindowFlags(mw::Window::getSdlWindow()) & SDL_WINDOW_MAXIMIZED)) {
								// The Window's is not maximized. Save position!
								int x, y;
								SDL_GetWindowPosition(mw::Window::getSdlWindow(), &x, &y);
								GameData::getInstance().setWindowPositionX(x);
								GameData::getInstance().setWindowPositionY(y);
							}
							break;
						case SDL_WINDOWEVENT_MAXIMIZED:
							GameData::getInstance().setWindowMaximized(true);
							break;
						case SDL_WINDOWEVENT_RESTORED:
							GameData::getInstance().setWindowMaximized(false);
							break;
					}
					break;
			}
		});
		zombieGame_ = std::make_shared<ZombieGame>();
		// Always react on key events!
		zombieGame_->setFocus(true);

		menuFrameIndex_ = getCurrentPanelIndex();
		playFrameIndex_ = addPanelBack();
		customGameFrameIndex_ = addPanelBack();
		highscoreFrameIndex_ = addPanelBack();

		initMenuFrame();
		initplayFrame();
		initCustomplayFrame();
		initHighscoreFrame();

		if (skipMenu_) {
			setCurrentPanel(playFrameIndex_);
		} else {
			setCurrentPanel(menuFrameIndex_);
		}

		setDefaultClosing(true);

		zombieGame_->addKeyListener(std::bind(&handleKeyboard, std::placeholders::_1, std::placeholders::_2));
	}

	void ZombieWindow::initMenuFrame() {
		setCurrentPanel(menuFrameIndex_);

		auto text = add<TransparentPanel>(gui::BorderLayout::NORTH)->addDefault<gui::Label>("Zombie", GameData::getInstance().getDefaultFont(80));
		text->setTextColor(1, 1, 1);
		
		auto panel = add<TransparentPanel>(gui::BorderLayout::WEST);
		panel->setPreferredSize(500, 500);
		panel->setLayout<gui::VerticalLayout>(10.f, 20.f, 60.f, gui::VerticalLayout::UP);

		ZombieWindow::getCurrentPanel()->setBackground(GameData::getInstance().getMenuBackgroundImage());
		
		panel->addDefault<Button>("Play", GameData::getInstance().getDefaultFont(35))->addActionListener([&](gui::Component&) {
			setCurrentPanel(playFrameIndex_);
			zombieGame_->startGame();
		});

		panel->addDefault<Button>("Custom game", GameData::getInstance().getDefaultFont(35))->addActionListener([&](gui::Component&) {
			GameData::getInstance().getMenuSoundChoice().play();
		});

		panel->addDefault<Button>("Highscore", GameData::getInstance().getDefaultFont(35))->addActionListener([&](gui::Component&) {
			GameData::getInstance().getMenuSoundChoice().play();
		});

		panel->addDefault<Button>("Quit", GameData::getInstance().getDefaultFont(35))->addActionListener([&](gui::Component&) {
			GameData::getInstance().getMenuSoundChoice().play();
			Window::quit();
		});
	}

	void ZombieWindow::initplayFrame() {
		setCurrentPanel(playFrameIndex_);
		add(gui::BorderLayout::CENTER, zombieGame_);
	}

	void ZombieWindow::initCustomplayFrame() {
		setCurrentPanel(customGameFrameIndex_);
	}

	void ZombieWindow::initHighscoreFrame() {
		setCurrentPanel(highscoreFrameIndex_);
	}

} // Namespace zombie.
