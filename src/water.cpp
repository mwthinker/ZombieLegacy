#include "water.h"

#include "box2ddef.h"
#include "auxiliary.h"

#include <mw/opengl.h>
#include <mw/color.h>
#include <mw/sprite.h>

#include <iostream>
#include <vector>

namespace zombie {

	Water::Triangle::Triangle(Position p1, Position p2, Position p3) : p1_(p1), p2_(p2), p3_(p3) {
	}

	Water::CircularWave::CircularWave(float amplitude, Position center, float waveLength, float speed) {
		amplitude_ = amplitude;
		center_ = center;
		waveLength_ = waveLength;
		speed_ = speed;
	}

	Water::Water(const mw::Texture& seeFloor) : seeFloor_(seeFloor) {
		time_ = 0.0;
		waterShader_ = std::make_shared<mw::Shader>();
		waterShader_->bindAttribute("aPos");
		waterShader_->bindAttribute("aTex");
		waterShader_->loadAndLinkFromFile("water.ver.glsl", "water.fra.glsl");
	}

	void Water::addTriangle(Position p1, Position p2, Position p3) {
		triangles_.emplace_back(p1, p2, p3);
	}

	void Water::draw(float deltaTime, gui::WindowMatrixPtr wp) {
		time_ += deltaTime;

		float size = 100.f;

		// Triangles.
		GLfloat aPos[400];
		GLfloat aTex[400];
		int index = 0;
		for (auto& triangle : triangles_) {
			aPos[index * 6 + 0] = triangle.p1_.x;
			aPos[index * 6 + 1] = triangle.p1_.y;
			aPos[index * 6 + 2] = triangle.p2_.x;
			aPos[index * 6 + 3] = triangle.p2_.y;
			aPos[index * 6 + 4] = triangle.p3_.x;
			aPos[index * 6 + 5] = triangle.p3_.y;
			aTex[index * 6 + 0] = 0;
			aTex[index * 6 + 1] = 0;
			aTex[index * 6 + 2] = (triangle.p2_.x - triangle.p1_.x) / size;
			aTex[index * 6 + 3] = (triangle.p2_.y - triangle.p1_.y) / size;
			aTex[index * 6 + 4] = (triangle.p3_.x - triangle.p1_.x) / size;
			aTex[index * 6 + 5] = (triangle.p3_.y - triangle.p1_.y) / size;
			++index;
		}

		// Draw see floor.
		mw::glEnable(GL_TEXTURE_2D);
		seeFloor_.bind();
		wp->useShader();
		wp->setColor(1, 1, 1);
		wp->setTexture(true);
		wp->setTexturePosition(2, aTex);
		wp->setVertexPosition(2, aPos);
		wp->setColor(0.3f, 0.3f, 0.3f);
		wp->glDrawArrays(GL_TRIANGLES, 0, index * 3);
		mw::glDisable(GL_TEXTURE_2D);

		// Draw waves.
		waterShader_->glUseProgram();
		mw::Matrix44 m = wp->getProjection() * wp->getModel();
		mw::glUniformMatrix4fv(waterShader_->getUniformLocation("uMat"), 1, false, m.data());
		mw::glUniform1f(waterShader_->getUniformLocation("uTime"), time_);

		mw::glEnableVertexAttribArray(waterShader_->getAttributeLocation("aTex"));
		mw::glVertexAttribPointer(waterShader_->getAttributeLocation("aTex"), 2, GL_FLOAT, GL_FALSE, 0, aTex);
		mw::glEnableVertexAttribArray(waterShader_->getAttributeLocation("aPos"));
		mw::glVertexAttribPointer(waterShader_->getAttributeLocation("aPos"), 2, GL_FLOAT, GL_FALSE, 0, aPos);

		mw::glEnable(GL_BLEND);
		mw::glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		mw::glDrawArrays(GL_TRIANGLES, 0, index * 3); // 3 vertices for every loop.

		mw::glDisable(GL_BLEND);
		wp->useShader();
	}

} // Namespace zombie.
