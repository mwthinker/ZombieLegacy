#ifndef MAPEDITORWINDOW_H
#define MAPEDITORWINDOW_H

#include "mw/gamewindow.h"
#include "mw/sprite.h"
#include "zombiegame.h"

#include <SDL_opengl.h>
#include <iostream>

namespace zombie {

class MapEditorWindow : public mw::GameWindow {
public:
    MapEditorWindow() : mw::GameWindow(500,500,"Zombie Map Editor","images/icon.bmp") {
        setResizable(false);
        setTimeStep(20);

		setUnicodeInputEnable(true);
		reshapeWindowsOpenGL();
    }

	~MapEditorWindow() {
	}
private:
    void graphicUpdate(Uint32 msDeltaTime) override {
		glPushMatrix();
		//glScaled(getWidth(),getHeight(),1);
		glPopMatrix();
    }

    void physicUpdate(Uint32 msDeltaTime) override {
    }

    void eventUpdate(const SDL_Event& windowEvent) override {
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
				break;
			case SDLK_PAGEDOWN:
				break;
			case SDLK_RETURN:				
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
};

} // namespace zombie

#endif // MAPEDITORWINDOW_H
