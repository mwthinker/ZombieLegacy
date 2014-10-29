#include "building2d.h"
#include "auxiliary.h"

#include <mw/opengl.h>

namespace zombie {

	Building2D::Building2D() : Building(ZERO, ZERO, ZERO, ZERO) {
	}

	Building2D::Building2D(Position p1, Position p2, Position p3, Position p4,
		const mw::Sprite& rightWall, const mw::Sprite& leftWall, const mw::Sprite& roof)
		: Building(p1, p2, p3, p4), leftWall_(leftWall), rightWall_(rightWall), roof_(roof) {

		height_ = 2 + random() * 3;
	}

	void Building2D::draw(float accumulator, float timeStep, const GameShader& gameShader) {
		mw::glEnable(GL_BLEND);
		mw::glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		const mw::Texture& texture = roof_.getTexture();
		// Centered square in ORIGO.
		texture.bindTexture();

		const auto& corners = getCorners();
		GLfloat aVertices[] = {
			corners[0].x, corners[0].y,
			corners[1].x, corners[1].y,
			corners[3].x, corners[3].y,
			corners[2].x, corners[2].y};

		// Map the sprite out from the texture.
		GLfloat aTexCoord[] = {
			roof_.getX() / texture.getWidth(), roof_.getY() / texture.getHeight(),
			(roof_.getX() + roof_.getWidth()) / texture.getWidth(), roof_.getY() / texture.getHeight(),
			roof_.getX() / texture.getWidth(), (roof_.getY() + roof_.getHeight()) / texture.getHeight(),
			(roof_.getX() + roof_.getWidth()) / texture.getWidth(), (roof_.getY() + roof_.getHeight()) / texture.getHeight()};

		// Load the vertex data.
		gameShader.setGlTextureU(true);
		gameShader.setGlPositionU(ZERO);
		gameShader.setGlAngleU(0);
		gameShader.setGlVer2dCoordsA(aVertices);
		gameShader.setGlTexCoordsA(aTexCoord);

		// Upload the attributes and draw the sprite.
		mw::glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		mw::glDisable(GL_BLEND);
	}

} // Namespace zombie.
