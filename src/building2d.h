#ifndef BUILDING2D_H
#define BUILDING2D_H

#include "building.h"

#include <mw/sprite.h>

#include <vector>

namespace zombie {

	class Building2D : public Building {
	public:
		Building2D();

		Building2D(Position p1, Position p2, Position p3, Position p4,
			const mw::Sprite& rightWall, const mw::Sprite& leftWall, const mw::Sprite& roof);

		void draw(float accumulator, float timeStep, const GameShader& gameShader);

		void drawLeftWall(const mw::Sprite& wall);

		void drawRightWall(const mw::Sprite& wall);

		void drawRoof(const mw::Sprite& roof, const GameShader& gameShader);

		void drawGTAstyle(const mw::Sprite& s);

	private:
		float height_;
		mw::Sprite leftWall_, rightWall_, roof_;
	};

} // Namespace zombie.

#endif // BUILDING2D_H
