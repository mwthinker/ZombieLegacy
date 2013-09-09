#ifndef SURVIVALTIMER_H
#define SURVIVALTIMER_H

#include "graphictask.h"
#include "gamefont.h"

#include <mw/text.h>

#include <sstream>
#include <cmath>

namespace zombie {

	class SurvivalTimer : public GraphicTask {
	public:
		SurvivalTimer() {
			dayTime_ = 60.0;
			day_ = mw::Text("", font15);
			hour_ = mw::Text("", font15);
		}

		bool draw(double time) override {
			glColor3d(1,1,1);			

			std::stringstream stream;
			stream << "Day "<< (int) (time/60.0) + 1;
			day_.setText(stream.str());
			stream.str("");
			double h = std::fmod(time,60.0)/60*24;
			if (h < 10) {
				stream << 0;
			}
			stream << (int) h << " h";
			hour_.setText(stream.str());			
			glPushMatrix();
			glMatrixMode(GL_PROJECTION);
			glPushMatrix();
			glLoadIdentity();
			//glOrtho(0,Task::width,0,Task::height,-1,1);
			double proportion = Task::width * 1.0 / Task::height;
			double width = proportion*500;
			double height = 500;
			glOrtho(0,width,0,height,-1,1);
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glColor4d(0.1,0.1,0.1,0.2 * (1 + std::cos(h/12.0*mw::PI)*0.5 ) );
			glBegin(GL_QUADS);
			glVertex2d(0,0);
			glVertex2d(width,0);
			glVertex2d(width,height);
			glVertex2d(0,height);
			glEnd();
			glDisable(GL_BLEND);
			glColor4d(1,1,1,1);
			
			glTranslated(width-90,height-30,0);
			day_.draw();
			glTranslated(30,-30,0);
			hour_.draw();

			glMatrixMode(GL_PROJECTION);
			glPopMatrix();
			glMatrixMode(GL_MODELVIEW);
			glPopMatrix();

			return true;
		}

	private:
		double dayTime_;
		mw::Text day_, hour_;
	};

} // Namespace zombie.

#endif // SURVIVALTIMER_H
