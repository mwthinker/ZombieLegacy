#ifndef LASER_H
#define LASER_H

#include "gameshader.h"

#include <mw/opengl.h>
#include <mw/sprite.h>
#include <mw/shader.h>
#include <mw/color.h>
#include <mw/vertexbufferobject.h>

#include <memory>

namespace zombie {

	class Laser;
	typedef std::shared_ptr<Laser> LaserPtr;

	LaserPtr loadLaser();

	class Laser {
	public:
		static const int VERTEX_SIZE = 4;
		static const int VERTEX_PER_SPRITE = 6;

		Laser(float height, const mw::Sprite& laser, const mw::Sprite& laserEnd, const mw::Sprite& laserOverlay,
			const mw::Sprite& laserOverlayStatic, const mw::Sprite& laserOverlayStaticEnd,
			const mw::Color& laserColor, const mw::Color& overlayColor, float staticSpeed);
		
		Laser(const Laser& laser);
		Laser& operator=(const Laser& laser);

		inline void setOverlayColor(const mw::Color& color) {
			overlayColor_ = color;
		}

		inline void setLaserColor(const mw::Color& color) {
			laserColor_ = color;
		}

		inline void setSpeed(float speed) {
			speed_ = speed;
		}

		void update(float length);
		void update(float x, float y);

		void draw(float deltaTime, const zombie::GameShader& gameShader);

	private:
		void update();

		float time_;
		float x_, y_;
		float length_;

		float spriteScale_;
		float speed_;
		float ratio_;

		std::array<GLfloat, 20 * VERTEX_PER_SPRITE * VERTEX_SIZE> data_;
		mw::Color overlayColor_;
		mw::Color laserColor_;

		mw::Sprite laser_;
		mw::Sprite laserEnd_;

		mw::Sprite laserOverlay_;

		mw::Sprite laserOverlayStatic_;
		mw::Sprite laserOverlayStaticEnd_;
		mw::VertexBufferObject vbo_;
	};

} // Namespace zombie.

#endif // LASER_H
