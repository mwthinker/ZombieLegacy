#ifndef GRAPHICENTITY_H
#define GRAPHICENTITY_H

#include "state.h"

namespace zombie {

	class GraphicEntity {
	public:
		virtual ~GraphicEntity() {
		}

		virtual void draw(float time, float timeStep, float accumulator) = 0;
	};

} // Namespace zombie.

#endif // GRAPHICENTITY_H
