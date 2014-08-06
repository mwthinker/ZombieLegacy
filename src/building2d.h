#ifndef BUILDING2D_H
#define BUILDING2D_H

#include <mw/texture.h>

#include "building.h"
#include "auxiliary.h"

#include <mw/opengl.h>

#include <vector>

namespace zombie {

	class Building2D : public Building {
	public:
		Building2D(const std::vector<Position>& corners, const mw::Sprite& wall) : Building(corners), wall_(wall) {
			height_ = 2 + random() * 3;
		}

		void draw(float accumulator, float timeStep) override {
			auto& corners = getCorners();
			// Draw building footprint (Use this for the pavement!)
			glColor3f(0.3f, 0.3f, 0.3f);
			glBegin(GL_POLYGON);
			for (unsigned int i = 0; i < 4 && i < corners.size(); ++i) {
				glVertex2d(corners[i].x, corners[i].y);
			}
			glEnd();
			
			// Draw fake 3D building with height (Without corner outlines).
			for (unsigned int i = 0; i < 3 && i < corners.size(); ++i) {
				glBegin(GL_POLYGON);
				glVertex2d(corners[i].x, corners[i].y);
				glVertex2d(corners[i + 1].x, corners[i + 1].y);
				glVertex2d(corners[i + 1].x, corners[i + 1].y + height_);
				glVertex2d(corners[i].x, corners[i].y + height_);
				glVertex2d(corners[i].x, corners[i].y);
				glEnd();
			}
			
			// Draw fake 3D building outlines with height.
			glColor3f(0.5f, 0.5f, 0.5f);
			glBegin(GL_LINES);
			glVertex2d(corners[0].x, corners[0].y);
			glVertex2d(corners[1].x, corners[1].y);
			
			glVertex2d(corners[0].x, corners[0].y);
			glVertex2d(corners[3].x, corners[3].y);
			
			glVertex2d(corners[0].x, corners[0].y + height_);
			glVertex2d(corners[1].x, corners[1].y + height_);
			
			glVertex2d(corners[0].x, corners[0].y + height_);
			glVertex2d(corners[3].x, corners[3].y + height_);
			
			glVertex2d(corners[3].x, corners[3].y);
			glVertex2d(corners[3].x, corners[3].y + height_);
			
			glVertex2d(corners[0].x, corners[0].y);
			glVertex2d(corners[0].x, corners[0].y + height_);
			
			glVertex2d(corners[1].x, corners[1].y);
			glVertex2d(corners[1].x, corners[1].y + height_);
			
			glVertex2d(corners[3].x, corners[3].y + height_);
			glVertex2d(corners[2].x, corners[2].y + height_);
			
			glVertex2d(corners[1].x, corners[1].y + height_);
			glVertex2d(corners[2].x, corners[2].y + height_);
			glEnd();
			
			// SPRITE EXPERIMENTS
			// Draw body.
			/*
			glPushMatrix();
			glTranslate2f(getPosition());
			glScale2f(2 * getRadius());
			glRotated(0, 0, 0, 1);
			glColor3d(1, 1, 1);
			wall_.draw();
			glPopMatrix();
			*/

			// RIGHT WALL
			const mw::Texture& texture = wall_.getTexture();
			wall_.bind();
			glEnable(GL_BLEND);
			glEnable(GL_TEXTURE_2D);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glNormal3f(0, 0, 1);
			glBegin(GL_QUADS);
			glTexCoord2f(wall_.getX() / texture.getWidth(), wall_.getY() / texture.getHeight());
			glVertex2f(corners[0].x, corners[0].y);
			glTexCoord2f((wall_.getX() + wall_.getWidth()) / texture.getWidth(), wall_.getY() / texture.getHeight());
			glVertex2f(corners[1].x, corners[1].y);
			glTexCoord2f((wall_.getX() + wall_.getWidth()) / texture.getWidth(), (wall_.getY() + wall_.getHeight()) / texture.getHeight());
			glVertex2f(corners[1].x, corners[1].y+height_);
			glTexCoord2f(wall_.getX() / texture.getWidth(), (wall_.getY() + wall_.getHeight()) / texture.getHeight());
			glVertex2f(corners[0].x, corners[0].y + height_);
			glEnd();
			glDisable(GL_TEXTURE_2D);
			glDisable(GL_BLEND);
			
			// LEFT WALL
			//const mw::Texture& texture = wall_.getTexture();
			wall_.bind();
			glEnable(GL_BLEND);
			glEnable(GL_TEXTURE_2D);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glNormal3f(0, 0, 1);
			glBegin(GL_QUADS);
			glTexCoord2f(wall_.getX() / texture.getWidth(), wall_.getY() / texture.getHeight());
			glVertex2f(corners[3].x, corners[3].y);
			glTexCoord2f((wall_.getX() + wall_.getWidth()) / texture.getWidth(), wall_.getY() / texture.getHeight());
			glVertex2f(corners[0].x, corners[0].y);
			glTexCoord2f((wall_.getX() + wall_.getWidth()) / texture.getWidth(), (wall_.getY() + wall_.getHeight()) / texture.getHeight());
			glVertex2f(corners[0].x, corners[0].y + height_);
			glTexCoord2f(wall_.getX() / texture.getWidth(), (wall_.getY() + wall_.getHeight()) / texture.getHeight());
			glVertex2f(corners[3].x, corners[3].y + height_);
			glEnd();
			glDisable(GL_TEXTURE_2D);
			glDisable(GL_BLEND);

			// ROOF
			//const mw::Texture& texture = wall_.getTexture();
			wall_.bind();
			glEnable(GL_BLEND);
			glEnable(GL_TEXTURE_2D);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glNormal3f(0, 0, 1);
			glBegin(GL_QUADS);
			glTexCoord2f(wall_.getX() / texture.getWidth(), wall_.getY() / texture.getHeight());
			glVertex2f(corners[0].x, corners[0].y+height_);
			glTexCoord2f((wall_.getX() + wall_.getWidth()) / texture.getWidth(), wall_.getY() / texture.getHeight());
			glVertex2f(corners[1].x, corners[1].y+height_);
			glTexCoord2f((wall_.getX() + wall_.getWidth()) / texture.getWidth(), (wall_.getY() + wall_.getHeight()) / texture.getHeight());
			glVertex2f(corners[2].x, corners[2].y + height_);
			glTexCoord2f(wall_.getX() / texture.getWidth(), (wall_.getY() + wall_.getHeight()) / texture.getHeight());
			glVertex2f(corners[3].x, corners[3].y + height_);
			glEnd();
			glDisable(GL_TEXTURE_2D);
			glDisable(GL_BLEND);
			
		}
	
	private:
		float height_;
		mw::Sprite wall_;
	};



} // Namespace zombie.

#endif // BUILDING2D_H
