#ifndef GRAPHIC_H
#define GRAPHIC_H

#include "gameshader.h"

namespace zombie {

	class Graphic {
	public:
		virtual void draw(float deltaTime, const GameShader& gameShader) = 0;

		virtual bool toBeRemoved() const = 0;		

	protected:
		virtual ~Graphic() {
		}
	};

} // Namespace zombie.

#endif // GRAPHIC_H
