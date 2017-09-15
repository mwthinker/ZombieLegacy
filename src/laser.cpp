#include "laser.h"
#include "gameshader.h"
#include "box2ddef.h"

#include <mw/opengl.h>
#include <mw/shader.h>
#include <mw/color.h>
#include <mw/texture.h>

namespace zombie {

	namespace {		

		// Add vertex data into the array given start at index.
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

		// Add vertex data for a square into the array given start at index.
		inline void addSquare(GLfloat* data, int& index,
			float x1, float y1,
			float x2, float y2,
			float xTex1, float yTex1,
			float xTex2, float yTex2) {

			// Left triangle.
			addTriangle(data, index,
				x1, y1,
				x2, y1,
				x1, y2,
				xTex1, yTex1,
				xTex2, yTex1,
				xTex1, yTex2);

			// Right triangle.
			addTriangle(data, index,
				x1, y2,
				x2, y1,
				x2, y2,
				xTex1, yTex2,
				xTex2, yTex1,
				xTex2, yTex2);
		}

		// Creates a mesh of vertices of repeated sprites. It is horizonted aligned.
		int repeatSprite(GLfloat* data, int& index, const int maxSize, const float x, const float y, const float ratio, const float length, const float scale, const mw::Sprite& sprite) {
			const mw::Texture& texture = sprite.getTexture();
			int vertices = 0;
			float x1 = x;
			float x2 = x + (ratio * sprite.getWidth()) * scale;
			const float y1 = y - 0.5f * sprite.getHeight()*scale;
			const float y2 = y + 0.5f * sprite.getHeight()*scale;

			if (index + Laser::VERTEX_PER_SPRITE*Laser::VERTEX_SIZE < maxSize) {
				addSquare(data, index,
					x1, y1,
					x2, y2,
					(sprite.getX() + (1 - ratio) * sprite.getWidth()) / texture.getWidth(), sprite.getY() / texture.getHeight(),
					(sprite.getX() + sprite.getWidth()) / texture.getWidth(), (sprite.getY() + sprite.getHeight()) / texture.getHeight());
				vertices += Laser::VERTEX_PER_SPRITE;
			}

			x1 = x2;
			x2 += sprite.getWidth() * scale;
			while (x2 < x + length && index + Laser::VERTEX_PER_SPRITE*Laser::VERTEX_SIZE < maxSize) {
				addSquare(data, index,
					x1, y1,
					x2, y2,
					sprite.getX() / texture.getWidth(), sprite.getY() / texture.getHeight(),
					(sprite.getX() + sprite.getWidth()) / texture.getWidth(), (sprite.getY() + sprite.getHeight()) / texture.getHeight());
				vertices += Laser::VERTEX_PER_SPRITE;

				x1 = x2;
				x2 += sprite.getWidth() * scale;
			}

			if (index + Laser::VERTEX_PER_SPRITE*Laser::VERTEX_SIZE < maxSize) {
				// x1 equal the last x2.
				x2 = x + length; // End point.
				addSquare(data, index,
					x1, y1,
					x2, y2,
					sprite.getX() / texture.getWidth(), sprite.getY() / texture.getHeight(),
					(sprite.getX() + sprite.getWidth() * (x2 - x1) / sprite.getWidth()) / texture.getWidth(), (sprite.getY() + sprite.getHeight()) / texture.getHeight());
				vertices += Laser::VERTEX_PER_SPRITE;
			}
			return vertices;
		}

	}

	LaserPtr loadLaser() {
		/*
		float speed = entry.getChildEntry("speed").getFloat();
		mw::Color overlayColor = entry.getChildEntry("overlayColor").getColor();
		mw::Color laserColor = entry.getChildEntry("laserColor").getColor();
		float height = entry.getChildEntry("height").getFloat();

		auto childEntry = entry.getChildEntry("sprites");		
		mw::Sprite laser = childEntry.getChildEntry("laser").getSprite();
		mw::Sprite laserEnd = childEntry.getChildEntry("laserEnd").getSprite();
		mw::Sprite laserOverlay = childEntry.getChildEntry("laserOverlay").getSprite();
		mw::Sprite laserOverlayStatic = childEntry.getChildEntry("laserOverlayStatic").getSprite();
		mw::Sprite laserOverlayStaticEnd = childEntry.getChildEntry("laserOverlayStaticEnd").getSprite();
		return std::make_shared<Laser>(height, laser, laserEnd, laserOverlay, laserOverlayStatic, laserOverlayStaticEnd, laserColor, overlayColor, speed);
		*/
		return nullptr;
	}

	Laser::Laser(float height, const mw::Sprite& laser, const mw::Sprite& laserEnd, const mw::Sprite& laserOverlay,
		const mw::Sprite& laserOverlayStatic, const mw::Sprite& laserOverlayStaticEnd,
		const mw::Color& laserColor, const mw::Color& overlayColor, float staticSpeed) : 
		spriteScale_(height / laserEnd.getHeight()), laser_(laser), laserEnd_(laserEnd), laserOverlay_(laserOverlay),
		laserOverlayStatic_(laserOverlayStatic), laserOverlayStaticEnd_(laserOverlayStaticEnd),
		laserColor_(laserColor), overlayColor_(overlayColor),
		speed_(staticSpeed) {
		
		time_ = 0;
		ratio_ = 0;
		length_ = 10;
		x_ = 0;
		y_ = 0;
	}

	Laser::Laser(const Laser& laser) : spriteScale_(laser.spriteScale_), laser_(laser.laser_), laserEnd_(laser.laserEnd_), laserOverlay_(laser.laserOverlay_),
		laserOverlayStatic_(laser.laserOverlayStatic_), laserOverlayStaticEnd_(laser.laserOverlayStaticEnd_),
		laserColor_(laser.laserColor_), overlayColor_(laser.overlayColor_), speed_(laser.speed_) {

		time_ = 0;
		ratio_ = 0;
		length_ = 10;
		x_ = 0;
		y_ = 0;
	}

	Laser& Laser::operator=(const Laser& laser) {
		spriteScale_ = laser.spriteScale_;
		laser_ = laser.laser_;
		laserEnd_ = laser.laserEnd_;
		laserOverlay_ = laser.laserOverlay_;
		laserOverlayStatic_ = laser.laserOverlayStatic_;
		laserOverlayStaticEnd_ = laser.laserOverlayStaticEnd_;
		laserColor_ = laser.laserColor_;
		overlayColor_ = laser.overlayColor_;
		speed_ = laser.speed_;

		time_ = 0;
		ratio_ = 0;
		length_ = 10;
		x_ = 0;
		y_ = 0;
		
		return *this;
	}

	void Laser::update(float length) {
		length_ = length;
	}

	void Laser::update(float x, float y) {
		x_ = x;
		y_ = y;
	}

	void Laser::update() {
		int index = 0;

		const mw::Texture& texture = laserEnd_.getTexture();
		addSquare(data_.data(), index,
			x_, y_ - 0.5f*laser_.getHeight()*spriteScale_,
			x_ + length_ - 0.5f*laserEnd_.getWidth()*spriteScale_, y_ + 0.5f*laser_.getHeight()*spriteScale_,
			laser_.getX() / texture.getWidth(), laser_.getY() / texture.getHeight(),
			(laser_.getX() + laser_.getWidth()) / texture.getWidth(), (laser_.getY() + laser_.getHeight()) / texture.getHeight());

		addSquare(data_.data(), index,
			x_ + length_ - 0.5f*laserEnd_.getWidth()*spriteScale_, y_ - 0.5f*laserEnd_.getHeight()*spriteScale_,
			x_ + length_ + 0.5f*laserEnd_.getWidth()*spriteScale_, y_ + 0.5f*laserEnd_.getHeight()*spriteScale_,
			laserEnd_.getX() / texture.getWidth(), laserEnd_.getY() / texture.getHeight(),
			(laserEnd_.getX() + laserEnd_.getWidth()) / texture.getWidth(), (laserEnd_.getY() + laserEnd_.getHeight()) / texture.getHeight());

		addSquare(data_.data(), index,
			x_, y_ - 0.5f*laser_.getHeight()*spriteScale_,
			x_ + length_ - 0.5f*laserOverlayStaticEnd_.getWidth()*spriteScale_, y_ + 0.5f*laserOverlayStatic_.getHeight()*spriteScale_,
			laserOverlayStatic_.getX() / texture.getWidth(), laserOverlayStatic_.getY() / texture.getHeight(),
			(laserOverlayStatic_.getX() + laserOverlayStatic_.getWidth()) / texture.getWidth(), (laserOverlayStatic_.getY() + laserOverlayStatic_.getHeight()) / texture.getHeight());

		addSquare(data_.data(), index,
			x_ + length_ - 0.5f*laserOverlayStaticEnd_.getWidth()*spriteScale_, y_ - 0.5f*laserOverlayStaticEnd_.getHeight()*spriteScale_,
			x_ + length_ + 0.5f*laserOverlayStaticEnd_.getWidth()*spriteScale_, y_ + 0.5f*laserOverlayStaticEnd_.getHeight()*spriteScale_,
			laserOverlayStaticEnd_.getX() / texture.getWidth(), laserOverlayStaticEnd_.getY() / texture.getHeight(),
			(laserOverlayStaticEnd_.getX() + laserOverlayStaticEnd_.getWidth()) / texture.getWidth(), (laserOverlayStaticEnd_.getY() + laserOverlayStaticEnd_.getHeight()) / texture.getHeight());
	}

	void Laser::draw(float deltaTime, const zombie::GameShader& gameShader) {
		vbo_.bindBufferData(GL_ARRAY_BUFFER, data_.size() * sizeof(GLfloat), data_.data(), GL_DYNAMIC_DRAW);
		vbo_.unbindBuffer();

		time_ += deltaTime;
		ratio_ += deltaTime * speed_;
		if (ratio_ > 1) {
			ratio_ = 0;
		}
		
		update();
		int index = VERTEX_PER_SPRITE * VERTEX_SIZE * 4; // 4 sprites before the repeated mesh in the data array.
		int repeatVertices = repeatSprite(data_.data(), index, data_.size(), x_, y_, ratio_, length_, spriteScale_, laserOverlay_);
		mw::checkGlError();
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		gameShader.useProgram();
		gameShader.setGlTextureU(true);
		
		// Binds and uploads the laser data.
		vbo_.bindBufferSubData(0, (4 * VERTEX_PER_SPRITE + repeatVertices) * VERTEX_SIZE * sizeof(GLfloat), data_.data());
		
		laser_.bindTexture();
		// Set the vertex data.
		gameShader.setGlVer2dCoordsA(sizeof(GLfloat) * VERTEX_SIZE, 0);
		gameShader.setGlTexCoordsA(sizeof(GLfloat) * VERTEX_SIZE, (GLvoid*) (sizeof(GLfloat) * 2));
		gameShader.setGlColorU(laserColor_);
		// Draw the first two sprites.
		glDrawArrays(GL_TRIANGLES, 0, VERTEX_PER_SPRITE * 2);

		gameShader.setGlColorU(overlayColor_);
		// Draw everything except the first two sprites.
		glDrawArrays(GL_TRIANGLES, VERTEX_PER_SPRITE * 2, repeatVertices + VERTEX_PER_SPRITE * 2);
		vbo_.unbindBuffer();

		glDisable(GL_BLEND);		
	}

} // Namespace zombie.
