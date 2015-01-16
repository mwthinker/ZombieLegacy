#ifndef TERRAIN2D_H
#define TERRAIN2D_H

#include "box2ddef.h"
#include "gameshader.h"
#include "zombieentry.h"

#include <mw/opengl.h>
#include <mw/vertexbufferobject.h>
#include <mw/texture.h>
#include <mw/sprite.h>

#include <vector>

namespace zombie {	

	class Terrain2D {
	public:
		Terrain2D() : 
			numberVerticesRoads_(0), numberVerticesGrass_(0) {
		}
		
		void loadRoadSprites(ZombieEntry entry);
		void addRoad(ZombieEntry tileEntry);
		void addGrass(Position p1, Position p2, Position p3);

		void draw(float time, const GameShader& shader);

	private:
		mw::Sprite intersection_;
		mw::Sprite straight0_, straight90_;
		mw::Sprite turn0_, turn90_, turn180_, turn270_;
		mw::Sprite tintersection0_, tintersection90_, tintersection180_, tintersection270_;

		mw::VertexBufferObject vbo_;
		int numberVerticesRoads_;
		int numberVerticesGrass_;

		std::vector<GLfloat> roads_;
		std::vector<GLfloat> grass_;
	};

} // Namespace zombie.

#endif // TERRAIN2D_H
