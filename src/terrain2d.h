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
			numberVertices_(0) {
			
		}

		void addRoad(Position p1, Position p2, Position p3) {
			roads_.push_back(p1.x); roads_.push_back(p1.y);
			roads_.push_back(p2.x); roads_.push_back(p2.y);
			roads_.push_back(p3.x); roads_.push_back(p3.y);
			++numberVertices_;
		}

		void draw(float time, const GameShader& shader) {
			if (vboRoads_.getSize() == 0) {
				vboRoads_.bindBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * roads_.size() + 100, roads_.data(), GL_STATIC_DRAW);
				roads_.clear();
			}

			vboRoads_.bindBuffer();
			
			shader.setGlTextureU(false);
			shader.setGlVer2dCoordsA((const void*) 0);
			shader.setGlColorU(0.1f, 0.1f, 0.1f);
			mw::glDrawArrays(GL_TRIANGLES, 0, numberVertices_);

			vboRoads_.unbindBuffer();
		}

	private:
		mw::VertexBufferObject vboRoads_;
		int numberVertices_;

		std::vector<GLfloat> roads_;
	};

} // Namespace zombie.

#endif // TERRAIN2D_H
