#ifndef WATER_H
#define WATER_H

#include "box2ddef.h"
#include "auxiliary.h"
#include "zombieentry.h"
#include "gameshader.h"
#include "watershader.h"

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
		Water(const mw::Sprite& seeFloor);

		void addTriangle(Position p1, Position p2, Position p3);
		
		void drawSeeFloor(float deltaTime, const GameShader& shader);

		void drawWaves(float deltaTime, const WaterShader& shader);

	private:
		struct CircularWave {
			CircularWave(float amplitude, Position center, float waveLength, float speed);

			float amplitude_;
			Position center_;
			float waveLength_;
			float speed_;
		};

		mw::VertexBufferObject vbo_;

		std::vector<GLfloat> aPos_;
		std::vector<GLfloat> aTex_;
		mw::Sprite seeFloor_;
		int numberVertices_;

		float floorTime_;
		float waveTime_;
		mw::Sprite water_;
		float size_;
	};

	Water loadWater(ZombieEntry entry);

} // Namespace zombie.

#endif // WATER_H
