#ifndef GAMEVERTEXDATA_H
#define GAMEVERTEXDATA_H

#include "gameshader.h"

#include <mw/vertexdata.h>
#include <mw/color.h>
#include <mw/sprite.h>

#include <vector>

namespace zombie {

	class GameVertexData;
	using GameVertexDataPtr = std::shared_ptr<GameVertexData>;

	class GameVertexData : public mw::VertexData {
	public:
		GameVertexData(const GameShader& gameShader) : gameShader_(gameShader) {
		}

		virtual ~GameVertexData() = default;

		// Add two triangles, GL_TRIANGLES, i.e. 6 vertices.
		void addSquareTRIANGLES(
			float x, float y,
			float w, float h,
			const mw::Color& color);

		void addSquareTRIANGLES(float x, float y, float w, float h, const mw::Sprite& sprite, const mw::Color& color = mw::Color(1, 1, 1));

		void addSquareTRIANGLES(float x1, float y1,
			float x2, float y2,
			float x3, float y3,
			float x4, float y4,
			const mw::Sprite& sprite, const mw::Color& color = mw::Color(1, 1, 1));

		void addVertex(float x, float y, float xTex, float yTex, bool isTex, const mw::Color& color);

		void addTriangleTRIANGLES(
			float x1, float y1,
			float x2, float y2,
			float x3, float y3,
			float xTex1, float yTex1,
			float xTex2, float yTex2,
			float xTex3, float yTex3,
			bool isTex,
			const mw::Color& color);

		void updateVertex(float x, float y);

		void updateSquareTRIANGLES(float x, float y, float w, float h);

		void updateTriangleTRIANGLES(float x1, float y1,
			float x2, float y2,
			float x3, float y3);

		void setVertexAttribPointer() const override {
			gameShader_.setVertexAttribPointer();
		}

		void useProgram() const override {
			gameShader_.useProgram();
		}

		unsigned int vertexSizeInFloat() const override {
			return gameShader_.vertexSizeInFloat();
		}

		void drawTRIANGLES() {
			drawMode(GL_TRIANGLES);
		}

	private:
		GameShader gameShader_;
	};

} // Namespace zombie.

#endif // GAMEVERTEXDATA_H
