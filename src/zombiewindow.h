#ifndef ZOMBIEWINDOW_H
#define ZOMBIEWINDOW_H

#include "zombiegame.h"

#include <mw/window.h>
#include <mw/sprite.h>

#include <tinyxml2.h>
#include <SDL_opengl.h>

#include <iostream>

namespace zombie {

	class ZombieWindow : public mw::Window {
	public:
		ZombieWindow(int width, int height, tinyxml2::XMLHandle xml) : mw::Window(width, height, true, "Zombie","images/icon.bmp"), zombieGame_(500, 500, xml), xml_(xml) {
			reshapeWindowsOpenGL();
		}

		~ZombieWindow() {
		}

	private:
		void update(Uint32 msDeltaTime) override {
			// Draw graphic.
			glPushMatrix();
			if (getWidth() > getHeight()) {
				double dist = (getWidth() - getHeight()) * 0.5;
				// Keep the view in centre.
				glTranslated(dist,0,0);
				// Scale correctly, keep proportions.
				glScaled(getHeight(),getHeight(),1);
			} else {
				double dist = (getHeight() - getWidth()) * 0.5;
				// Keep the view in centre.
				glTranslated(0,dist,0);
				// Scale correctly, keep proportions.
				glScaled(getWidth(),getWidth(),1);
			}
			zombieGame_.update(msDeltaTime/1000.f);

			glPopMatrix();
		}

		void eventUpdate(const SDL_Event& windowEvent) override {
			zombieGame_.eventUpdate(windowEvent);

			switch (windowEvent.type) {
			case SDL_QUIT:
				quit();
				break;
			case SDL_WINDOWEVENT:
				switch (windowEvent.window.event) {
				case SDL_WINDOWEVENT_CLOSE:
					quit();
					break;
				case SDL_WINDOWEVENT_RESIZED:
					resize(windowEvent.window.data1, windowEvent.window.data2);
					break;
				default:
					break;
				}
				break;
			case SDL_KEYDOWN:
				switch (windowEvent.key.keysym.sym) {
				case SDLK_ESCAPE:
					quit();
					break;
				case SDLK_PAGEUP:
					zombieGame_.zoom(1.1);
					break;
				case SDLK_PAGEDOWN:
					zombieGame_.zoom(1/1.1);
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

		void resize(int width, int height) {
			reshapeWindowsOpenGL();
			zombieGame_.updateSize(width,height);
		}

		void reshapeWindowsOpenGL() {
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			int w = getWidth();
			int h = getHeight();

			glViewport(0,0,w,h);
			glOrtho(0,w,0,h,-1,1);
			std::cout << "\nreshapeWindowsOpenGL" << std::endl;

			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
		}

		ZombieGame zombieGame_;
		tinyxml2::XMLHandle xml_;
	};

} // Namespace zombie.

#endif // ZOMBIEWINDOW_H
