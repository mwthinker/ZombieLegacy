#include "building2d.h"
#include "auxiliary.h"

#include <mw/opengl.h>

namespace zombie {

		Building2D::Building2D(const std::vector<Position>& corners,
			const mw::Sprite& rightWall, const mw::Sprite& leftWall, const mw::Sprite& roof)
			: Building(corners), leftWall_(leftWall), rightWall_(rightWall), roof_(roof) {
			
			height_ = 2 + random() * 3;
			leftWall_.setDrawFunction(std::bind(&Building2D::drawLeftWall, this, leftWall_));
			rightWall_.setDrawFunction(std::bind(&Building2D::drawRightWall, this, rightWall_));
			//roof_.setDrawFunction(std::bind(&Building2D::drawRoof, this, roof_));
			roof_.setDrawFunction(std::bind(&Building2D::drawGTAstyle, this, roof_));
		}

		void Building2D::draw(float accumulator, float timeStep) {
			auto& corners = getCorners();
			glEnable(GL_BLEND);
			glEnable(GL_TEXTURE_2D);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			
			glColor3f(1, 1, 1);
			//leftWall_.draw();
			//rightWall_.draw();
			roof_.draw();
			glDisable(GL_TEXTURE_2D);
			glDisable(GL_BLEND);
		}

		void Building2D::drawLeftWall(const mw::Sprite& wall) {
			const mw::Texture& texture = wall.getTexture();
			auto& corners = getCorners();
			wall.bind();
			glBegin(GL_QUADS);
			glTexCoord2f(wall.getX() / texture.getWidth(), wall.getY() / texture.getHeight());
			glVertex2f(corners[3].x, corners[3].y);
			glTexCoord2f((wall.getX() + wall.getWidth()) / texture.getWidth(), wall.getY() / texture.getHeight());
			glVertex2f(corners[0].x, corners[0].y);
			glTexCoord2f((wall.getX() + wall.getWidth()) / texture.getWidth(), (wall.getY() + wall.getHeight()) / texture.getHeight());
			glVertex2f(corners[0].x, corners[0].y + height_);
			glTexCoord2f(wall.getX() / texture.getWidth(), (wall.getY() + wall.getHeight()) / texture.getHeight());
			glVertex2f(corners[3].x, corners[3].y + height_);
			glEnd();
		}

		void Building2D::drawRightWall(const mw::Sprite& wall) {
			const mw::Texture& texture = wall.getTexture();
			auto& corners = getCorners();
			wall.bind();
			glEnable(GL_BLEND);
			glEnable(GL_TEXTURE_2D);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glNormal3f(0, 0, 1);
			glBegin(GL_QUADS);
			glTexCoord2f(wall.getX() / texture.getWidth(), wall.getY() / texture.getHeight());
			glVertex2f(corners[0].x, corners[0].y);
			glTexCoord2f((wall.getX() + wall.getWidth()) / texture.getWidth(), wall.getY() / texture.getHeight());
			glVertex2f(corners[1].x, corners[1].y);
			glTexCoord2f((wall.getX() + wall.getWidth()) / texture.getWidth(), (wall.getY() + wall.getHeight()) / texture.getHeight());
			glVertex2f(corners[1].x, corners[1].y + height_);
			glTexCoord2f(wall.getX() / texture.getWidth(), (wall.getY() + wall.getHeight()) / texture.getHeight());
			glVertex2f(corners[0].x, corners[0].y + height_);
			glEnd();
		}

		void Building2D::drawRoof(const mw::Sprite& roof) {
			const mw::Texture& texture = roof.getTexture();
			auto& corners = getCorners();
			roof.bind();
			glBegin(GL_QUADS);
			glTexCoord2f(roof.getX() / texture.getWidth(), roof.getY() / texture.getHeight());
			glVertex2f(corners[0].x, corners[0].y + height_);
			glTexCoord2f((roof.getX() + roof.getWidth()) / texture.getWidth(), roof.getY() / texture.getHeight());
			glVertex2f(corners[1].x, corners[1].y + height_);
			glTexCoord2f((roof.getX() + roof.getWidth()) / texture.getWidth(), (roof.getY() + roof.getHeight()) / texture.getHeight());
			glVertex2f(corners[2].x, corners[2].y + height_);
			glTexCoord2f(roof.getX() / texture.getWidth(), (roof.getY() + roof.getHeight()) / texture.getHeight());
			glVertex2f(corners[3].x, corners[3].y + height_);
			glEnd();
		}

		void Building2D::drawGTAstyle(const mw::Sprite& s) {
			float c = 0.1f;
			const mw::Texture& texture = s.getTexture();
			auto& corners = getCorners();
			
			// RIGHT WALL
			s.bind();
			glBegin(GL_QUADS);
			glTexCoord2f(s.getX() / texture.getWidth(), s.getY() / texture.getHeight());
			glVertex2f(corners[0].x, corners[0].y);
			glTexCoord2f((s.getX() + s.getWidth()) / texture.getWidth(), s.getY() / texture.getHeight());
			glVertex2f(corners[1].x, corners[1].y);
			glTexCoord2f((s.getX() + s.getWidth()) / texture.getWidth(), (s.getY() + s.getHeight()) / texture.getHeight());
			float dx = -humanPosition().x + corners[1].x;
			float dy = -humanPosition().y + corners[1].y;
			glVertex2f(corners[1].x+dx*c, corners[1].y+dy*c);
			glTexCoord2f(s.getX() / texture.getWidth(), (s.getY() + s.getHeight()) / texture.getHeight());
			dx = -humanPosition().x + corners[0].x;
			dy = -humanPosition().y + corners[0].y;
			glVertex2f(corners[0].x + dx*c, corners[0].y + dy*c);
			glEnd();

			// LEFT WALL
			s.bind();
			glBegin(GL_QUADS);
			glTexCoord2f(s.getX() / texture.getWidth(), s.getY() / texture.getHeight());
			glVertex2f(corners[3].x, corners[3].y);
			glTexCoord2f((s.getX() + s.getWidth()) / texture.getWidth(), s.getY() / texture.getHeight());
			glVertex2f(corners[0].x, corners[0].y);
			glTexCoord2f((s.getX() + s.getWidth()) / texture.getWidth(), (s.getY() + s.getHeight()) / texture.getHeight());
			dx = -humanPosition().x + corners[0].x;
			dy = -humanPosition().y + corners[0].y;
			glVertex2f(corners[0].x + dx*c, corners[0].y + dy*c);
			glTexCoord2f(s.getX() / texture.getWidth(), (s.getY() + s.getHeight()) / texture.getHeight());
			dx = -humanPosition().x + corners[3].x;
			dy = -humanPosition().y + corners[3].y;
			glVertex2f(corners[3].x + dx*c, corners[3].y + dy*c);
			glEnd();

			// RIGHT WALL BACK
			s.bind();
			glBegin(GL_QUADS);
			glTexCoord2f(s.getX() / texture.getWidth(), s.getY() / texture.getHeight());
			glVertex2f(corners[1].x, corners[1].y);
			glTexCoord2f((s.getX() + s.getWidth()) / texture.getWidth(), s.getY() / texture.getHeight());
			glVertex2f(corners[2].x, corners[2].y);
			glTexCoord2f((s.getX() + s.getWidth()) / texture.getWidth(), (s.getY() + s.getHeight()) / texture.getHeight());
			dx = -humanPosition().x + corners[2].x;
			dy = -humanPosition().y + corners[2].y;
			glVertex2f(corners[2].x + dx*c, corners[2].y + dy*c);
			glTexCoord2f(s.getX() / texture.getWidth(), (s.getY() + s.getHeight()) / texture.getHeight());
			dx = -humanPosition().x + corners[1].x;
			dy = -humanPosition().y + corners[1].y;
			glVertex2f(corners[1].x + dx*c, corners[1].y + dy*c);
			glEnd();

			// LEFT WALL BACK
			s.bind();
			glBegin(GL_QUADS);
			glTexCoord2f(s.getX() / texture.getWidth(), s.getY() / texture.getHeight());
			glVertex2f(corners[2].x, corners[2].y);
			glTexCoord2f((s.getX() + s.getWidth()) / texture.getWidth(), s.getY() / texture.getHeight());
			glVertex2f(corners[3].x, corners[3].y);
			glTexCoord2f((s.getX() + s.getWidth()) / texture.getWidth(), (s.getY() + s.getHeight()) / texture.getHeight());
			dx = -humanPosition().x + corners[3].x;
			dy = -humanPosition().y + corners[3].y;
			glVertex2f(corners[3].x + dx*c, corners[3].y + dy*c);
			glTexCoord2f(s.getX() / texture.getWidth(), (s.getY() + s.getHeight()) / texture.getHeight());
			dx = -humanPosition().x + corners[2].x;
			dy = -humanPosition().y + corners[2].y;
			glVertex2f(corners[2].x + dx*c, corners[2].y + dy*c);
			glEnd();

			// ROOF
			s.bind();
			glBegin(GL_QUADS);
			glTexCoord2f(s.getX() / texture.getWidth(), s.getY() / texture.getHeight());
			dx = -humanPosition().x  + corners[0].x;
			dy = -humanPosition().y + corners[0].y;
			glVertex2f(corners[0].x+dx*c, corners[0].y+dy*c);
			glTexCoord2f((s.getX() + s.getWidth()) / texture.getWidth(), s.getY() / texture.getHeight());
			dx = -humanPosition().x + corners[1].x;
			dy = -humanPosition().y + corners[1].y;
			glVertex2f(corners[1].x+dx*c, corners[1].y+dy*c);
			glTexCoord2f((s.getX() + s.getWidth()) / texture.getWidth(), (s.getY() + s.getHeight()) / texture.getHeight());
			dx = -humanPosition().x + corners[2].x;
			dy = -humanPosition().y + corners[2].y;
			glVertex2f(corners[2].x + dx*c, corners[2].y + dy*c);
			glTexCoord2f(s.getX() / texture.getWidth(), (s.getY() + s.getHeight()) / texture.getHeight());
			dx = -humanPosition().x + corners[3].x;
			dy = -humanPosition().y + corners[3].y;
			glVertex2f(corners[3].x + dx*c, corners[3].y + dy*c);
			glEnd();
		}

} // Namespace zombie.
