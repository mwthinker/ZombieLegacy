#ifndef BUILDING2D_H
#define BUILDING2D_H

#include "building.h"

#include <mw/sprite.h>

#include <vector>

namespace zombie {

	class Building2D : public Building {
	public:
		Building2D(const std::vector<Position>& corners,
			const mw::Sprite& rightWall, const mw::Sprite& leftWall, const mw::Sprite& roof);

		void draw(float accumulator, float timeStep) override;

		void drawLeftWall(const mw::Sprite& wall);

		void drawRightWall(const mw::Sprite& wall);

		void drawRoof(const mw::Sprite& roof);

		void drawGTAstyle(const mw::Sprite& s);

	private:
		float height_;
		mw::Sprite leftWall_, rightWall_, roof_;
	};



} // Namespace zombie.

#endif // BUILDING2D_H
