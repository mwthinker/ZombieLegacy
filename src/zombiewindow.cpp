#include "zombiewindow.h"
#include "zombiegame.h"
#include "zombieentry.h"

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

	ZombieWindow::ZombieWindow(ZombieEntry zombieEntry, bool skipMenu) : zombieEntry_(zombieEntry), skipMenu_(skipMenu) {
		mw::Window::setPosition(zombieEntry_.getDeepChildEntry("window positionX").getInt(), zombieEntry_.getDeepChildEntry("window positionY").getInt());
		mw::Window::setWindowSize(zombieEntry_.getDeepChildEntry("window width").getInt(), zombieEntry_.getDeepChildEntry("window height").getInt());
		mw::Window::setIcon("images/1.png");
		mw::Window::setResizeable(true);
		mw::Window::setFullScreen(zombieEntry_.getDeepChildEntry("window maximized").getBool());
	}

	ZombieWindow::~ZombieWindow() {
		zombieEntry_.save();
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
								zombieEntry_.getChildEntry("window").getChildEntry("width").setInt(sdlEvent.window.data1);
								zombieEntry_.getChildEntry("window").getChildEntry("height").setInt(sdlEvent.window.data2);
							}
							break;
						case SDL_WINDOWEVENT_MOVED:
							if (!(SDL_GetWindowFlags(mw::Window::getSdlWindow()) & SDL_WINDOW_MAXIMIZED)) {
								// The Window's is not maximized. Save position!
								int x, y;
								SDL_GetWindowPosition(mw::Window::getSdlWindow(), &x, &y);
								zombieEntry_.getChildEntry("window").getChildEntry("positionX").setInt(x);
								zombieEntry_.getChildEntry("window").getChildEntry("positionY").setInt(y);
							}
							break;
						case SDL_WINDOWEVENT_MAXIMIZED:
							zombieEntry_.getChildEntry("window").getChildEntry("maximized").setBool(true);
							break;
						case SDL_WINDOWEVENT_RESTORED:
							zombieEntry_.getChildEntry("window").getChildEntry("maximized").setBool(false);
							break;
					}
					break;
			}
		});
		zombieGame_ = std::make_shared<ZombieGame>(zombieEntry_);
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

		auto text = add<TransparentPanel>(gui::BorderLayout::NORTH)->addDefault<gui::Label>("Zombie", zombieEntry_.getDeepChildEntry("interface font").getFont(80));
		text->setTextColor(1, 1, 1);
		
		auto panel = add<TransparentPanel>(gui::BorderLayout::WEST);
		panel->setPreferredSize(500, 500);
		panel->setLayout<gui::VerticalLayout>(10.f, 20.f, 60.f, gui::VerticalLayout::UP);

		ZombieWindow::getCurrentPanel()->setBackground(zombieEntry_.getDeepChildEntry("menu backGroundImage").getSprite());

		panel->setBackground(zombieEntry_.getDeepChildEntry("menu backGroundImage").getSprite());
		mw::Font font = zombieEntry_.getDeepChildEntry("interface font").getFont(35);
		
		panel->addDefaultToGroup<Button>("Play", font)->addActionListener([&](gui::Component&) {
			setCurrentPanel(playFrameIndex_);
			zombieGame_->startGame();
		});

		panel->addDefaultToGroup<Button>("Custom game", font)->addActionListener([&](gui::Component&) {
			zombieEntry_.getDeepChildEntry("menu soundChoice").getSound().play();
		});

		panel->addDefaultToGroup<Button>("Highscore", font)->addActionListener([&](gui::Component&) {
			zombieEntry_.getDeepChildEntry("menu soundChoice").getSound().play();
		});

		panel->addDefaultToGroup<Button>("Quit", font)->addActionListener([&](gui::Component&) {
			zombieEntry_.getDeepChildEntry("menu soundChoice").getSound().play();
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
