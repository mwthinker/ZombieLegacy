#ifndef MAPTASK_H
#define MAPTASK_H

#include <vector>

namespace zombie {

	class MapTask {
	public:
		MapTask();
		virtual ~MapTask() {
		}

		virtual void drawFirst(double time) {
		}		
		
		virtual void drawSecond(double time) {
		}

		virtual void drawThird(double time) {
		}

		virtual bool isRunning() const = 0;
	};

} // Namespace zombie.

#endif // MAPTASK_H