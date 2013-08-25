#ifndef GRAPHICTASK_H
#define GRAPHICTASK_H

#include "graphictask.h"
#include "map.h"
#include "gamesprite.h"
#include "unit.h"
#include "building.h"
#include "typedefs.h"
#include "weaponobject.h"

#include <mw/text.h>
#include <mw/signal.h>
#include <SDL_opengl.h>

#include <vector>
#include <queue>

namespace zombie {

	class GraphicTask {
	public:
		virtual ~GraphicTask() {
		}

		virtual void draw(double time) = 0;
		virtual bool isRunning() const = 0;
	};

} // Namespace zombie.

#endif // GRAPHICTASK_H
