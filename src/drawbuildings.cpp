#include "drawbuildings.h"
#include "auxiliary.h"

#include <mw/opengl.h>

#include <vector>

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

	DrawBuildings::DrawBuildings() {
		nbrBuildings_ = 0;
	}

	void DrawBuildings::createVBO(const Array<Building2D, 100>& buildings, const mw::Texture& buildingsTexture) {
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

	void DrawBuildings::drawWall(float accumulator, float timeStep, const BuildingShader& gameShader) {
		//drawArrays(6, 0, gameShader);
	}

	void DrawBuildings::drawRoof(float accumulator, float timeStep, const BuildingShader& gameShader) {
		drawArrays(0, 6 * nbrBuildings_, gameShader); // // 6 (vertices) * 1 (roof) * nbrBuildings.
	}

	void DrawBuildings::drawArrays(int index, int vertices, const BuildingShader& gameShader) {
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

} // Namespace zombie.
