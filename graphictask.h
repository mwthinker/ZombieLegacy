#ifndef GRAPHICTASK_H
#define GRAPHICTASK_H

namespace zombie {

	class GraphicTask {
	public:
		virtual ~GraphicTask() {
		}

		virtual bool draw(double time) = 0;
	};

} // Namespace zombie.

#endif // GRAPHICTASK_H
