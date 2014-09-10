#ifndef WATER_H
#define WATER_H

#include "box2ddef.h"
#include "auxiliary.h"

#include <mw/opengl.h>
#include <mw/color.h>
#include <mw/sprite.h>
#include <mw/shader.h>
#include <gui/windowmatrix.h>

#include <iostream>
#include <vector>

namespace zombie {

	class Water {
	public:
		Water(const mw::Texture& seeFloor);

		void addTriangle(Position p1, Position p2, Position p3);

		void drawSeeFloor(float deltaTime, gui::WindowMatrixPtr wp);

		void drawWaves(const mw::Matrix44& matrix);

	private:
		struct CircularWave {
			CircularWave(float amplitude, Position center, float waveLength, float speed);

			float amplitude_;
			Position center_;
			float waveLength_;
			float speed_;
		};

		mw::ShaderPtr waterShader_;		

		std::vector<GLfloat> aPos_;
		std::vector<GLfloat> aTex_;
		mw::Texture seeFloor_;

		float time_;
		mw::Sprite water_;
		float size_;
	};

} // Namespace zombie.

#endif // WATER_H
