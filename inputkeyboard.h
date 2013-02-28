#ifndef INPUTKEYBOARD_H
#define INPUTKEYBOARD_H

#include "humanplayer.h"

#include <SDL.h>

namespace zombie {

class InputKeyboard : public HumanPlayer {
public:
	InputKeyboard(SDLKey up, SDLKey down, SDLKey left, SDLKey right, SDLKey shoot, SDLKey reload, SDLKey run) {		
		up_ = up;
		down_ = down;
		right_ = right;
		left_ = left;
		shoot_ = shoot;
		reload_ = reload;
		run_ = run;
	}

    void eventUpdate(const SDL_Event& windowEvent) override {
		SDLKey key = windowEvent.key.keysym.sym;
		
        switch (windowEvent.type) {
        case SDL_KEYDOWN:
			if (key == up_) {
				input_.forward_ = true;
			} else if (key == down_) {
				input_.backward_ = true;
			} else if (key == left_) {
				input_.turnLeft_ = true;
			} else if (key == right_) {
				input_.turnRight_ = true;
			} else if (key == shoot_) {
				input_.shoot_ = true;
			} else if (key == reload_) {
				input_.reload_ = true;
			} else if (key == run_) {
				input_.run_ = true;
			}
            break;
        case SDL_KEYUP:
			if (key == up_) {
				input_.forward_ = false;
			} else if (key == down_) {
				input_.backward_ = false;
			} else if (key == left_) {
				input_.turnLeft_ = false;
			} else if (key == right_) {
				input_.turnRight_ = false;
			} else if (key == run_) {
				input_.run_ = false;
			}
            break;
        default:
            break;
        }
    }

	Input currentInput() override {
		Input input = input_;
		input_.shoot_  = false;
		input_.reload_ = false;
		return input;
	}
private:
	Input input_;
	SDLKey up_, down_, right_, left_, shoot_, reload_, run_;
};

} // namespace zombie

#endif // INPUTKEYBOARD_H