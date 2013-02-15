#ifndef ZOMBIEWINDOW_H
#define ZOMBIEWINDOW_H

#include "mw/gamewindow.h"
#include "mw/sprite.h"
#include "zombiegame.h"

#include <SDL_opengl.h>
#include <iostream>

namespace zombie {

class ZombieWindow : public mw::GameWindow {
public:
    ZombieWindow() : mw::GameWindow(500,500,"Zombie","images/icon.bmp") {
        setResizable(false);
        setTimeStep(20);

		setUnicodeInputEnable(true);
		reshapeWindowsOpenGL();
    }

	~ZombieWindow() {
	}
private:
    void graphicUpdate(Uint32 msDeltaTime) override {
		glPushMatrix();
		double scale = getWidth()*1.0/getHeight();
		double scaledW = getWidth() / zombieGame_.getWidth();
		//double scaledH = getHeight() / zombieGame_.getHeight();
		//glScaled(scale * scaledW, scaledW,1);
		//glScaled(1.5, 1.5, 1);
		zombieGame_.graphicUpdate(msDeltaTime);
		glPopMatrix();
    }

    void physicUpdate(Uint32 msDeltaTime) override {		
		zombieGame_.physicUpdate(msDeltaTime);
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
			case SDLK_F2:
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
    }

    void reshapeWindowsOpenGL() {
		//glEnable(GL_LINE_SMOOTH);
		glHint(GL_LINE_SMOOTH_HINT,GL_NICEST);
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
