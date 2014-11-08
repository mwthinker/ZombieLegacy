#ifndef DRAWBUILDINGS_H
#define DRAWBUILDINGS_H

#include "building2d.h"
#include "buildingshader.h"
#include "array.h"

#include <mw/texture.h>
#include <mw/vertexbufferobject.h>

namespace zombie {

	class DrawBuildings {
	public:
		DrawBuildings();

		void createVBO(const Array<Building2D, 100>& buildings, const mw::Texture& buildingsTexture);

		void drawWall(float accumulator, float timeStep, const BuildingShader& gameShader);

		void drawRoof(float accumulator, float timeStep, const BuildingShader& gameShader);

	private:
		void drawArrays(int index, int verticesconst, const BuildingShader& gameShader);

		mw::VertexBufferObject vbo_;
		mw::Texture buildingsTexture_;
		int nbrBuildings_;
	};

} // Namespace zombie.

#endif // DRAWBUILDINGS_H
