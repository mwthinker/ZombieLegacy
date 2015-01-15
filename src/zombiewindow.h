#ifndef ZOMBIEWINDOW_H
#define ZOMBIEWINDOW_H

#include "zombieentry.h"

#include <gui/frame.h>
#include <gui/label.h>

#include <memory>
#include <sstream>

namespace zombie {

	class GameData;
	class ZombieGame;

	class ZombieWindow : public gui::Frame {
	public:
		ZombieWindow(ZombieEntry entry, bool skipMenu = false);

		~ZombieWindow();

	private:
        void initMenuFrame();
        void initplayFrame();
        void initCustomplayFrame();
        void initHighscoreFrame();

        int menuFrameIndex_;
        int playFrameIndex_;
        int customGameFrameIndex_;
        int highscoreFrameIndex_;

		std::shared_ptr<ZombieGame> zombieGame_;
		std::shared_ptr<gui::Panel> gamePanel_;
		ZombieEntry zombieEntry_;
		std::shared_ptr<gui::Label> label1_,label2_,label3_,label4_;
		std::shared_ptr<gui::Label> fps_;
	};

} // Namespace zombie.

#endif // ZOMBIEWINDOW_H
