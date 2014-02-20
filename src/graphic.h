#ifndef GRAPHIC_H
#define GRAPHIC_H

#include "box2ddef.h"

#include <mw/sprite.h>

namespace zombie {

	class Graphic {
	public:
		virtual void draw(float deltaTime) = 0;

		virtual bool toBeRemoved() const = 0;

	protected:
		virtual ~Graphic() {
		}
	};

} // Namespace zombie.

#endif // GRAPHIC_H
