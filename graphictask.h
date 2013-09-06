#ifndef GRAPHICTASK_H
#define GRAPHICTASK_H

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
