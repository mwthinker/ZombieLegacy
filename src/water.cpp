#include "water.h"
#include "box2ddef.h"
#include "auxiliary.h"
#include "gamedataentry.h"

#include <mw/opengl.h>
#include <mw/color.h>
#include <mw/sprite.h>

#include <iostream>
#include <vector>

namespace zombie {

	Water::CircularWave::CircularWave(float amplitude, Position center, float waveLength, float speed) {
		amplitude_ = amplitude;
		center_ = center;
		waveLength_ = waveLength;
		speed_ = speed;
	}

	Water::Water(const mw::Texture& seeFloor) : seeFloor_(seeFloor) {
		time_ = 0.0;
		waterShader_.bindAttribute("aPos");
		waterShader_.bindAttribute("aTex");
		waterShader_.loadAndLinkFromFile("water.ver.glsl", "water.fra.glsl");
		size_ = 100;
		numberVertices_ = 0;

		waterShader_.glUseProgram();
		uProjIndex_ = waterShader_.getUniformLocation("uProj");
		uModelIndex_ = waterShader_.getUniformLocation("uModel");
		uPosIndex_ = waterShader_.getUniformLocation("uPos");
		uTimeIndex_ = waterShader_.getUniformLocation("uTime");
		aPosIndex_ = waterShader_.getAttributeLocation("aPos");
		aTexIndex_ = waterShader_.getAttributeLocation("aTex");
	}

	void Water::addTriangle(Position p1, Position p2, Position p3) {
		aPos_.push_back(p1.x); aPos_.push_back(p1.y);
		aPos_.push_back(p2.x); aPos_.push_back(p2.y);
		aPos_.push_back(p3.x); aPos_.push_back(p3.y);
		
		aTex_.push_back(0); aTex_.push_back(0);
		aTex_.push_back((p2.x - p1.x) / size_); aTex_.push_back((p2.y - p1.y) / size_);
		aTex_.push_back((p3.x - p1.x) / size_); aTex_.push_back((p3.y - p1.y) / size_);
		numberVertices_ += 3;
	}

	void Water::drawSeeFloor(float deltaTime, const GameShader& shader) {
		time_ += deltaTime;
		
		if (vbo_.getSize() == 0) {
			aPos_.insert(aPos_.end(), aTex_.begin(), aTex_.end());
			vbo_.bindBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * aPos_.size(), aPos_.data(), GL_STATIC_DRAW);
			aPos_.clear();
			aTex_.clear();
		}
		
		shader.useGlShader();
		shader.setGlTextureU(true);
		shader.setGlColorU(1, 1, 1);
		shader.setGlAngleU(0);
		shader.setGlPositionU(ZERO);

		// Draw see floor.
		vbo_.bindBuffer();
		
		seeFloor_.bindTexture();
		shader.setGlVer2dCoordsA((const void*) 0);
		shader.setGlTexCoordsA((const void*) (vbo_.getSize() / 2));
		mw::glDrawArrays(GL_TRIANGLES, 0, numberVertices_);
		
		vbo_.unbindBuffer();
	}

	void Water::drawWaves() {
		// Draw waves.
		waterShader_.glUseProgram();
		vbo_.bindBuffer();
		mw::glUniform1f(uTimeIndex_, time_);

		mw::glEnableVertexAttribArray(aPosIndex_);
		mw::glVertexAttribPointer(aPosIndex_, 2, GL_FLOAT, GL_FALSE, 0, (const void*) 0);
		mw::glEnableVertexAttribArray(aTexIndex_);
		mw::glVertexAttribPointer(aTexIndex_, 2, GL_FLOAT, GL_FALSE, 0, (const void*) (vbo_.getSize() / 2));

		mw::glEnable(GL_BLEND);
		mw::glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		mw::glDrawArrays(GL_TRIANGLES, 0, numberVertices_); // 3 vertices for every loop.
		mw::glDisable(GL_BLEND);
		
		vbo_.unbindBuffer();
	}

	void Water::updateShaderProjectionMatrix(const mw::Matrix44& proj) {
		waterShader_.glUseProgram();
		mw::glUniformMatrix4fv(uProjIndex_, 1, false, proj.data());
	}

	void Water::updateShaderModelMatrix(const mw::Matrix44& model) {
		waterShader_.glUseProgram();
		mw::glUniformMatrix4fv(uModelIndex_, 1, false, model.data());
	}

	void Water::updateShaderCenterPosition(Position position) {
		waterShader_.glUseProgram();
		mw::glUniform2f(uPosIndex_, position.x, position.y);
	}

	Water loadWater(GameDataEntry entry) {
		return Water(entry.getChildEntry("seeFloorImage").getTexture());
	}

} // Namespace zombie.
