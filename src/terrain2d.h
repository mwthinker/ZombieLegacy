#ifndef TERRAIN2D_H
#define TERRAIN2D_H

#include "box2ddef.h"

#include <mw/opengl.h>
#include <mw/shader.h>
#include <mw/vertexbufferobject.h>

#include <vector>

namespace zombie {

	class Terrain2D {
	public:
		Terrain2D() : 
			numberVerticesRoads_(0), numberVerticesGrass_(0) {
		}

		void addRoad(Position p1, Position p2, Position p3) {
			roads_.push_back(p1.x); roads_.push_back(p1.y);
			roads_.push_back(p2.x); roads_.push_back(p2.y);
			roads_.push_back(p3.x); roads_.push_back(p3.y);
			++numberVerticesRoads_;
		}

		void addGrass(Position p1, Position p2, Position p3) {
			grass_.push_back(p1.x); roads_.push_back(p1.y);
			grass_.push_back(p2.x); roads_.push_back(p2.y);
			grass_.push_back(p3.x); roads_.push_back(p3.y);
			++numberVerticesGrass_;
		}

		void draw(float time, const GameShader& shader) {
			if (vboRoads_.getSize() == 0) {
				roads_.insert(roads_.end(), grass_.begin(), grass_.end());
				vboRoads_.bindBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * roads_.size(), roads_.data(), GL_STATIC_DRAW);
				roads_.clear();
				grass_.clear();
			}

			shader.glUseProgram();
			vboRoads_.bindBuffer();
			
			shader.setGlTextureU(false);
			shader.setGlVer2dCoordsA((const void*) 0);
			shader.setGlColorU(0.1f, 0.1f, 0.1f);
			mw::glDrawArrays(GL_TRIANGLES, 0, numberVerticesRoads_);
			shader.setGlColorU(0.01f, 0.1f, 0.01f);
			mw::glDrawArrays(GL_TRIANGLES, numberVerticesRoads_, numberVerticesGrass_);

			vboRoads_.unbindBuffer();
		}

	private:
		mw::VertexBufferObject vboRoads_;
		int numberVerticesRoads_;
		int numberVerticesGrass_;

		std::vector<GLfloat> roads_;
		std::vector<GLfloat> grass_;
	};

} // Namespace zombie.

#endif // TERRAIN2D_H
