#include "building2d.h"
#include "auxiliary.h"

#include <mw/opengl.h>

namespace zombie {

	namespace {

		inline void addVertex(GLfloat* data, int& index,
			float x, float y, float z,
			float xTex, float yTex) {

			data[++index] = x;
			data[++index] = y;
			data[++index] = z;

			data[++index] = xTex;
			data[++index] = yTex;
		}

		// Add a triangle, GL_TRIANGLES, i.e. 3 vertices.
		inline void addTriangle(GLfloat* data, int& index,
			float x1, float y1, float z1,
			float x2, float y2, float z2,
			float x3, float y3, float z3,
			float xTex1, float yTex1,
			float xTex2, float yTex2,
			float xTex3, float yTex3) {

			addVertex(data, index, x1, y1, z1, xTex1, yTex1);
			addVertex(data, index, x2, y2, z2, xTex2, yTex2);
			addVertex(data, index, x3, y3, z3, xTex3, yTex3);
		}		

	}

	DrawBuilding2D::DrawBuilding2D() {
		nbrBuildings_ = 0;
	}

	void DrawBuilding2D::createVBO(const Array<Building2D, 100>& buildings, const mw::Texture& buildingsTexture) {
		buildingsTexture_ = buildingsTexture;
		vbo_ = mw::VertexBufferObject(); // Removes old vbo.

		// 5 (floats/vertex) * 6 (vertex/quad) * 1 (quads/3D-qube) * 1 (3D-qube, without bottom) * N (numberOfHouses).
		std::vector<GLfloat> data(5 * 6 * 1 * buildings.getSize());
		float height = 0.2f;
		nbrBuildings_ = buildings.getSize();

		int index = -1;
		for (const Building2D& building : buildings) {
			auto corners = building.getCorners();

			addTriangle(data.data(), index,
				corners[0].x, corners[0].y, height,
				corners[1].x, corners[1].y, height,
				corners[3].x, corners[3].y, height,
				building.roof_.getX() / buildingsTexture.getWidth(), building.roof_.getY() / buildingsTexture.getHeight(),
				(building.roof_.getX() + building.roof_.getWidth()) / buildingsTexture.getWidth(), building.roof_.getY() / buildingsTexture.getHeight(),
				building.roof_.getX() / buildingsTexture.getWidth(), (building.roof_.getY() + building.roof_.getHeight()) / buildingsTexture.getHeight());

			addTriangle(data.data(), index,
				corners[1].x, corners[1].y, height,
				corners[2].x, corners[2].y, height,
				corners[3].x, corners[3].y, height,
				(building.roof_.getX() + building.roof_.getWidth()) / buildingsTexture.getWidth(), building.roof_.getY() / buildingsTexture.getHeight(),
				(building.roof_.getX() + building.roof_.getWidth()) / buildingsTexture.getWidth(), (building.roof_.getY() + building.roof_.getHeight()) / buildingsTexture.getHeight(),
				building.roof_.getX() / buildingsTexture.getWidth(), (building.roof_.getY() + building.roof_.getHeight()) / buildingsTexture.getHeight());
		}
				
		vbo_.bindBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * data.size(), data.data(), GL_STATIC_DRAW);
	}

	void DrawBuilding2D::drawWall(float accumulator, float timeStep, const BuildingShader& gameShader) {
		//drawArrays(6, 0, gameShader);
	}

	void DrawBuilding2D::drawRoof(float accumulator, float timeStep, const BuildingShader& gameShader) {
		drawArrays(0, 6 * nbrBuildings_, gameShader); // // 6 (vertices) * 1 (roof) * nbrBuildings.
	}

	void DrawBuilding2D::drawArrays(int index, int vertices, const BuildingShader& gameShader) {
		if (vbo_.getSize() > 0) {
			mw::glEnable(GL_BLEND);
			mw::glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		
			// Centered square in ORIGO.
			gameShader.useGlShader();
			vbo_.bindBuffer();
			buildingsTexture_.bindTexture();

			// Load the vertex data.
			gameShader.setGlVer3dCoordsA(5 * sizeof(GLfloat), (const GLvoid*) 0);
			gameShader.setGlTexCoordsA(5 * sizeof(GLfloat), (const GLvoid*) (3 * sizeof(GLfloat)));
				
			// Upload the attributes and draw the sprite.
			mw::glDrawArrays(GL_TRIANGLES, index, vertices);
		
			vbo_.unbindBuffer();
			mw::glDisable(GL_BLEND);
		}
	}	

	Building2D::Building2D() : Building(ZERO, ZERO, ZERO, ZERO) {
	}

	Building2D::Building2D(Position p1, Position p2, Position p3, Position p4,
		const mw::Sprite& rightWall, const mw::Sprite& leftWall, const mw::Sprite& roof)
		: Building(p1, p2, p3, p4), leftWall_(leftWall), rightWall_(rightWall), roof_(roof),
		height_(0.2f) {

	}

} // Namespace zombie.
