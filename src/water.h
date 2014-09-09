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

		void draw(float deltaTime, gui::WindowMatrixPtr wp);

	private:
		struct CircularWave {
			CircularWave(float amplitude, Position center, float waveLength, float speed);

			float amplitude_;
			Position center_;
			float waveLength_;
			float speed_;
		};

		struct Triangle {
			Triangle(Position p1, Position p2, Position p3);

			Position p1_, p2_, p3_;
		};

		mw::ShaderPtr waterShader_;

		std::vector<Triangle> triangles_;
		mw::Texture seeFloor_;

		float time_;
		mw::Sprite water_;
	};

} // Namespace zombie.

#endif // WATER_H
