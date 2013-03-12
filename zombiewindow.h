#ifndef ZOMBIEWINDOW_H
#define ZOMBIEWINDOW_H

#include "zombiegame.h"

#include <mw/gamewindow.h>
#include <mw/sprite.h>

#include <SDL_opengl.h>
#include <iostream>

namespace zombie {

class ZombieWindow : public mw::Window {
public:
    ZombieWindow() : mw::Window(500,500,"Zombie","images/icon.bmp"), zombieGame_(500,500) {
        setResizable(true);
		setUnicodeInputEnable(true);
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
			setQuiting(true);
			break;
        case SDL_KEYDOWN:
            switch (windowEvent.key.keysym.sym) {
            case SDLK_t:
                mw::Window::setWindowsSize(100,100);
                //SDL_SetVideoMode(300,300, 32, SDL_OPENGL);
                //SDL_WM_ToggleFullScreen();
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
			case SDLK_ESCAPE:
				setQuiting(true);
				break;
            default:
                break;
			}
			break;
        default:
            break;
		}
	}
	
    void resize(int width, int height) override {
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
};

} // namespace zombie

#endif // ZOMBIEWINDOW_H
