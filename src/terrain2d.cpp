#include "terrain2d.h"

#include <mw/sprite.h>

#include <array>

namespace zombie {

	namespace {
		
		inline void addVertex(GLfloat* data, int& index,
			float x, float y,
			float xTex, float yTex) {

			data[index++] = x;
			data[index++] = y;

			data[index++] = xTex;
			data[index++] = yTex;
		}

		// Add a triangle, GL_TRIANGLES, i.e. 3 vertices.
		inline void addTriangle(GLfloat* data, int& index,
			float x1, float y1,
			float x2, float y2,
			float x3, float y3,
			float xTex1, float yTex1,
			float xTex2, float yTex2,
			float xTex3, float yTex3) {

			addVertex(data, index, x1, y1, xTex1, yTex1);
			addVertex(data, index, x2, y2, xTex2, yTex2);
			addVertex(data, index, x3, y3, xTex3, yTex3);
		}

		inline void addSquare(GLfloat* data, int& index,
			float x, float y,
			float w, float h, 
			const mw::Sprite& sprite) {
			const mw::Texture& texture = sprite.getTexture();
			float tW = (float) texture.getWidth();
			float tH = (float) texture.getHeight();

			// Left triangle |_
			addTriangle(data, index,
				x, y,
				x + w, y,
				x, y + h,
				sprite.getX() / tW, sprite.getY() / tH,
				(sprite.getX() + sprite.getWidth()) / tW, sprite.getY() / tH,
				sprite.getX() / tW, (sprite.getY() + sprite.getHeight()) / tH);
			//                _
			// Right triangle  |
			addTriangle(data, index,
				x, y + h,
				x + w, y,
				x + w, y + h,
				sprite.getX() / tW, (sprite.getY() + sprite.getHeight()) / tH,
				(sprite.getX() + sprite.getWidth()) / tW, sprite.getY() / tH,
				(sprite.getX() + sprite.getWidth()) / tW, (sprite.getY() + sprite.getHeight()) / tH);
		}
	}

	void Terrain2D::loadRoadSprites(GameDataEntry entry) {
		intersection_ = entry.getChildEntry("intersection").getSprite();
		straight0_ = entry.getChildEntry("straight0").getSprite();
		straight90_ = entry.getChildEntry("straight90").getSprite();
		turn0_ = entry.getChildEntry("turn0").getSprite();
		turn90_ = entry.getChildEntry("turn90").getSprite();
		turn180_ = entry.getChildEntry("turn180").getSprite();
		turn270_ = entry.getChildEntry("turn270").getSprite();
		tintersection0_ = entry.getChildEntry("tintersection0").getSprite();
		tintersection90_ = entry.getChildEntry("turntintersection90").getSprite();
		tintersection180_ = entry.getChildEntry("tintersection180").getSprite();
		tintersection270_ = entry.getChildEntry("tintersection270").getSprite();
	}

	void Terrain2D::addRoad(GameDataEntry tileEntry) {
		Position position = loadPoint(tileEntry.getChildEntry("geom").getString());
		std::string tileId = tileEntry.getChildEntry("tile_id").getString();
		int deg = tileEntry.getChildEntry("rotation").getInt();
		float size = tileEntry.getChildEntry("size").getFloat();
		mw::Sprite sprite;
		if (tileId == "turn") {
			switch (deg) {
				case 0:
					sprite = turn0_;
					break;
				case 90:
					sprite = turn90_;
					break;
				case 180:
					sprite = turn180_;
					break;
				case 270:
					sprite = turn270_;
					break;
			}
		} else if (tileId == "straight") {
			switch (deg) {
				case 0:
					sprite = straight0_;
					break;
				case 90:
					sprite = straight90_;
					break;
			}
		} else if (tileId == "tintersection") {
			switch (deg) {
				case 0:
					sprite = tintersection0_;
					break;
				case 90:
					sprite = tintersection90_;
					break;
				case 180:
					sprite = tintersection180_;
					break;
				case 270:
					sprite = tintersection270_;
					break;
			}
		} else { //if (tileId == "intersection") {
			sprite = intersection_;
		}
		
		// 4 float/vertices * 6 vertices.
		std::array<GLfloat, 4 * 6> data_;

		int index = 0;
		addSquare(data_.data(), index, position.x - size*0.5f, position.y - size*0.5f, size, size, sprite);
		roads_.insert(roads_.end(), data_.begin(), data_.end());
		numberVerticesRoads_ += 6;
	}

	void Terrain2D::addGrass(Position p1, Position p2, Position p3) {
		// 4 float/vertices * 3 vertices.
		std::array<GLfloat, 4 * 3> data_;
		int index = 0;
		addTriangle(data_.data(), index, p1.x, p1.y, p2.x, p2.y, p3.x, p3.y, 0, 0, 0, 0, 0, 0);
		grass_.insert(grass_.end(), data_.begin(), data_.end());
		numberVerticesGrass_ += 3;
	}

	void Terrain2D::draw(float time, const GameShader& shader) {
		mw::glEnable(GL_BLEND);
		mw::glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		if (vbo_.getSize() == 0) {
			grass_.insert(grass_.end(), roads_.begin(), roads_.end());
			vbo_.bindBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * grass_.size(), grass_.data(), GL_STATIC_DRAW);
			roads_.clear();
			grass_.clear();
		}

		shader.glUseProgram();
		vbo_.bindBuffer();

		shader.setGlVer2dCoordsA(4 * sizeof(GLfloat), (const GLvoid*) 0);
		shader.setGlTexCoordsA(4 * sizeof(GLfloat), (const GLvoid*) (sizeof(GLfloat) * 2));

		// No texture used for the grass.
		shader.setGlLocalAngleU(0);
		shader.setGlTextureU(false);
		shader.setGlColorU(0.01f, 0.1f, 0.01f);
		mw::glDrawArrays(GL_TRIANGLES, 0, numberVerticesGrass_);

		// Roads texture used.
		shader.setGlTextureU(true);
		shader.setGlColorU(1, 1, 1);
		mw::glDrawArrays(GL_TRIANGLES, numberVerticesGrass_, numberVerticesRoads_);

		vbo_.unbindBuffer();
		mw::glDisable(GL_BLEND);
	}

}
