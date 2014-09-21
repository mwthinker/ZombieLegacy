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
		waterShader_ = std::make_shared<mw::Shader>();
		waterShader_->bindAttribute("aPos");
		waterShader_->bindAttribute("aTex");
		waterShader_->loadAndLinkFromFile("water.ver.glsl", "water.fra.glsl");
		size_ = 100;
		numberVertices_ = 0;
	}

	void Water::addTriangle(Position p1, Position p2, Position p3) {
		aPos_.push_back(p1.x);
		aPos_.push_back(p1.y);
		aPos_.push_back(p2.x);
		aPos_.push_back(p2.y);
		aPos_.push_back(p3.x);
		aPos_.push_back(p3.y);
		aTex_.push_back(0);
		aTex_.push_back(0);
		aTex_.push_back((p2.x - p1.x) / size_);
		aTex_.push_back((p2.y - p1.y) / size_);
		aTex_.push_back((p3.x - p1.x) / size_);
		aTex_.push_back((p3.y - p1.y) / size_);
		numberVertices_ += 3;
	}

	void Water::drawSeeFloor(float deltaTime, gui::WindowMatrixPtr wp) {
		time_ += deltaTime;
		
		if (vbo_.getSize() == 0) {
			aPos_.insert(aPos_.end(), aTex_.begin(), aTex_.end());
			vbo_.bindBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * aPos_.size(), aPos_.data(), GL_STATIC_DRAW);
			aPos_.clear();
			aTex_.clear();
		}
				
		// Draw see floor.
		vbo_.bindBuffer();
		
		seeFloor_.bindTexture();
		wp->useShader();
		wp->setColor(1, 1, 1);
		wp->setTexture(true);
		wp->setVertexPosition(2, (const void*) 0);
		wp->setTexturePosition(2, (const void*) (vbo_.getSize() / 2));
		wp->setColor(0.3f, 0.3f, 0.3f);
		wp->glDrawArrays(GL_TRIANGLES, 0, numberVertices_);
		wp->setColor(1, 1, 1);
		vbo_.unbindBuffer();
	}

	void Water::drawWaves(const mw::Matrix44& matrix) {
		if (vbo_.getSize() == 0) {
			aPos_.insert(aPos_.end(), aTex_.begin(), aTex_.end());
			vbo_.bindBufferData(GL_ARRAY_BUFFER, aPos_.size(), aPos_.data(), GL_STATIC_DRAW);
			aPos_.clear();
			aTex_.clear();
		}

		// Draw waves.
		waterShader_->glUseProgram();
		vbo_.bindBuffer();
		mw::glUniformMatrix4fv(waterShader_->getUniformLocation("uMat"), 1, false, matrix.data());
		mw::glUniform1f(waterShader_->getUniformLocation("uTime"), time_);

		mw::glEnableVertexAttribArray(waterShader_->getAttributeLocation("aPos"));
		mw::glVertexAttribPointer(waterShader_->getAttributeLocation("aPos"), 2, GL_FLOAT, GL_FALSE, 0, (const void*) 0);
		mw::glEnableVertexAttribArray(waterShader_->getAttributeLocation("aTex"));
		mw::glVertexAttribPointer(waterShader_->getAttributeLocation("aTex"), 2, GL_FLOAT, GL_FALSE, 0, (const void*) (vbo_.getSize() / 2));

		mw::glEnable(GL_BLEND);
		mw::glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		mw::glDrawArrays(GL_TRIANGLES, 0, numberVertices_); // 3 vertices for every loop.
		mw::glDisable(GL_BLEND);
		
		vbo_.unbindBuffer();
	}

	Water loadWater(GameDataEntry entry) {
		return Water(entry.getChildEntry("seeFloorImage").getTexture());
	}

} // Namespace zombie.
