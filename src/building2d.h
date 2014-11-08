#ifndef BUILDING2D_H
#define BUILDING2D_H

#include "building.h"
#include "buildingshader.h"
#include "array.h"

#include <mw/sprite.h>
#include <mw/vertexbufferobject.h>

#include <vector>

namespace zombie {	

	class Building2D : public Building {
	public:
		friend class DrawBuilding2D;

		Building2D();

		Building2D(Position p1, Position p2, Position p3, Position p4,
			const mw::Sprite& rightWall, const mw::Sprite& leftWall, const mw::Sprite& roof);

	private:
		float height_;
		mw::Sprite leftWall_, rightWall_, roof_;
	};

	class DrawBuilding2D {
	public:
		DrawBuilding2D();

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

#endif // BUILDING2D_H
