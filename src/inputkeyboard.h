#ifndef INPUTKEYBOARD_H
#define INPUTKEYBOARD_H

#include "device.h"

#include <SDL.h>

namespace zombie {

	class InputKeyboard : public Device {
	public:
		InputKeyboard(SDL_Keycode up, SDL_Keycode down, SDL_Keycode left, SDL_Keycode right, SDL_Keycode shoot, SDL_Keycode reload, SDL_Keycode run, SDL_Keycode action) {
			up_ = up;
			down_ = down;
			right_ = right;
			left_ = left;
			shoot_ = shoot;
			reload_ = reload;
			run_ = run;
			action_ = action;
		}

		void eventUpdate(const SDL_Event& windowEvent) override {
			SDL_Keycode key = windowEvent.key.keysym.sym;

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
				} else if (key == action_) {
					input_.action_ = true;
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

		Input nextInput() override {
			Input input = input_;
			input_.shoot_  = false;
			input_.reload_ = false;
			input_.action_ = false;
			return input;
		}

	private:
		Input input_;
		SDL_Keycode up_, down_, right_, left_, shoot_, reload_, run_, action_;
	};

} // Namespace zombie.

#endif // INPUTKEYBOARD_H
