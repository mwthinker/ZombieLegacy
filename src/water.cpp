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

	Water::Water(const mw::Sprite& seeFloor) : seeFloor_(seeFloor) {
		floorTime_ = 0.0;
		waveTime_ = 0.0;
		size_ = 100;
		numberVertices_ = 0;
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
		floorTime_ += deltaTime;
		
		if (vbo_.getSize() == 0) {
			aPos_.insert(aPos_.end(), aTex_.begin(), aTex_.end());
			vbo_.bindBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * aPos_.size(), aPos_.data(), GL_STATIC_DRAW);
			aPos_.clear();
			aTex_.clear();
		}
		
		shader.glUseProgram();
		shader.setGlTextureU(true);
		shader.setGlColorU(1, 1, 1);
		shader.setGlLocalAngleU(0);
		shader.setGlGlobalPositionU(ZERO);

		// Draw see floor.
		vbo_.bindBuffer();
		
		seeFloor_.bindTexture();
		shader.setGlVer2dCoordsA((const void*) 0);
		shader.setGlTexCoordsA((const void*) (vbo_.getSize() / 2));
		mw::glDrawArrays(GL_TRIANGLES, 0, numberVertices_);
		
		vbo_.unbindBuffer();
	}

	void Water::drawWaves(float deltaTime, const WaterShader& shader) {
		waveTime_ += deltaTime;

		// Draw waves.
		shader.glUseProgram();
		vbo_.bindBuffer();

		shader.setGlTimeU(waveTime_);
		shader.setGlVer2dCoordsA((const void*) 0);

		mw::glEnable(GL_BLEND);
		mw::glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		mw::glDrawArrays(GL_TRIANGLES, 0, numberVertices_); // 3 vertices for every loop.
		mw::glDisable(GL_BLEND);
		
		vbo_.unbindBuffer();
	}

	Water loadWater(GameDataEntry entry) {
		return Water(entry.getChildEntry("seeFloorImage").getSprite());
	}

} // Namespace zombie.
