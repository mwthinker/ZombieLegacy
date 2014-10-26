#ifndef WATER_H
#define WATER_H

#include "box2ddef.h"
#include "auxiliary.h"
#include "gamedataentry.h"
#include "gameshader.h"

#include <mw/opengl.h>
#include <mw/color.h>
#include <mw/sprite.h>
#include <mw/shader.h>
#include <mw/vertexbufferobject.h>

#include <iostream>
#include <vector>

namespace zombie {	

	class Water {
	public:
		Water(const mw::Texture& seeFloor);

		void addTriangle(Position p1, Position p2, Position p3);
		
		void drawSeeFloor(float deltaTime, const GameShader& shader);

		void drawWaves();

		void updateShaderProjectionMatrix(const mw::Matrix44& proj);
		void updateShaderModelMatrix(const mw::Matrix44& model);

	private:
		struct CircularWave {
			CircularWave(float amplitude, Position center, float waveLength, float speed);

			float amplitude_;
			Position center_;
			float waveLength_;
			float speed_;
		};

		mw::Shader waterShader_;
		int uProjIndex_;
		int uModelIndex_;
		int uTimeIndex_;

		mw::VertexBufferObject vbo_;

		std::vector<GLfloat> aPos_;
		std::vector<GLfloat> aTex_;
		mw::Texture seeFloor_;
		int numberVertices_;

		float time_;
		mw::Sprite water_;
		float size_;
	};

	Water loadWater(GameDataEntry entry);

} // Namespace zombie.

#endif // WATER_H
