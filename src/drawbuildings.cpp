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

		// 5 (floats/vertex) * 6 (vertex/quad) * 5 (quads/3D-qube) * 1 (3D-qube, without bottom) * N (numberOfHouses).
		std::vector<GLfloat> data(5 * 6 * 5 * buildings.getSize());
		nbrBuildings_ = buildings.getSize();

		int index = -1;
		// Add roofs.
		for (const Building2D& building : buildings) {
			auto corners = building.getCorners();
			Position middle = ZERO;

			// Calculate the middle position.
			for (Position& p : corners) {
				middle += 0.25f * p;
			}

			// Roof corners.
			Position p1 = 0.2f * (middle - corners[0]) + corners[0];
			Position p2 = 0.2f * (middle - corners[1]) + corners[1];
			Position p3 = 0.2f * (middle - corners[2]) + corners[2];
			Position p4 = 0.2f * (middle - corners[3]) + corners[3];

			addTriangle(data.data(), index,
				p1.x, p1.y, building.getHeight(),
				p2.x, p2.y, building.getHeight(),
				p4.x, p4.y, building.getHeight(),
				building.roof_.getX() / buildingsTexture.getWidth(), building.roof_.getY() / buildingsTexture.getHeight(),
				(building.roof_.getX() + building.roof_.getWidth()) / buildingsTexture.getWidth(), building.roof_.getY() / buildingsTexture.getHeight(),
				building.roof_.getX() / buildingsTexture.getWidth(), (building.roof_.getY() + building.roof_.getHeight()) / buildingsTexture.getHeight());

			addTriangle(data.data(), index,
				p2.x, p2.y, building.getHeight(),
				p3.x, p3.y, building.getHeight(),
				p4.x, p4.y, building.getHeight(),
				(building.roof_.getX() + building.roof_.getWidth()) / buildingsTexture.getWidth(), building.roof_.getY() / buildingsTexture.getHeight(),
				(building.roof_.getX() + building.roof_.getWidth()) / buildingsTexture.getWidth(), (building.roof_.getY() + building.roof_.getHeight()) / buildingsTexture.getHeight(),
				building.roof_.getX() / buildingsTexture.getWidth(), (building.roof_.getY() + building.roof_.getHeight()) / buildingsTexture.getHeight());
		}
		// Add walls.
		for (const Building2D& building : buildings) {
			auto corners = building.getCorners();

			Position middle = ZERO;
			// Calculate the middle position.
			for (Position& p : corners) {
				middle += 0.25f * p;
			}

			// Roof corners.
			Position roof[4];
			roof[0] = 0.2f * (middle - corners[0]) + corners[0];
			roof[1] = 0.2f * (middle - corners[1]) + corners[1];
			roof[2] = 0.2f * (middle - corners[2]) + corners[2];
			roof[3] = 0.2f * (middle - corners[3]) + corners[3];

			for (int i = 0; i < 4; ++i) {
				addTriangle(data.data(), index,
					corners[0 + i].x, corners[0 + i].y, 0,
					corners[(1 + i) % 4].x, corners[(1 + i) % 4].y, 0,
					roof[0 + i].x, roof[0 + i].y, building.getHeight(),
					building.roof_.getX() / buildingsTexture.getWidth(), building.roof_.getY() / buildingsTexture.getHeight(),
					(building.roof_.getX() + building.roof_.getWidth()) / buildingsTexture.getWidth(), building.roof_.getY() / buildingsTexture.getHeight(),
					building.roof_.getX() / buildingsTexture.getWidth(), (building.roof_.getY() + building.roof_.getHeight()) / buildingsTexture.getHeight());

				addTriangle(data.data(), index,
					roof[0 + i].x, roof[0 + i].y, building.getHeight(),
					corners[(1 + i) % 4].x, corners[(1 + i) % 4].y, 0,
					roof[(1 + i) % 4].x, roof[(1 + i) % 4].y, building.getHeight(),
					(building.roof_.getX() + building.roof_.getWidth()) / buildingsTexture.getWidth(), building.roof_.getY() / buildingsTexture.getHeight(),
					(building.roof_.getX() + building.roof_.getWidth()) / buildingsTexture.getWidth(), (building.roof_.getY() + building.roof_.getHeight()) / buildingsTexture.getHeight(),
					building.roof_.getX() / buildingsTexture.getWidth(), (building.roof_.getY() + building.roof_.getHeight()) / buildingsTexture.getHeight());
			}
		}
				
		vbo_.bindBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * data.size(), data.data(), GL_STATIC_DRAW);
	}

	void DrawBuildings::drawWalls(float accumulator, float timeStep, const BuildingShader& gameShader) {
		drawArrays(6 * nbrBuildings_, 6 * nbrBuildings_ * 4, gameShader); // 6 (vertices) * 4 (walls) * nbrBuildings.
	}

	void DrawBuildings::drawRoofs(float accumulator, float timeStep, const BuildingShader& gameShader) {
		drawArrays(0, 6 * nbrBuildings_, gameShader); // 6 (vertices) * 1 (roof) * nbrBuildings.
	}

	void DrawBuildings::drawArrays(int index, int vertices, const BuildingShader& gameShader) {
		if (vbo_.getSize() > 0) {
			mw::glEnable(GL_BLEND);
			mw::glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		
			// Centered square in ORIGO.
			gameShader.glUseProgram();
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
