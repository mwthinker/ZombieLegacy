#ifndef GRAPHICOBJECT_H
#define GRAPHICOBJECT_H

namespace zombie {

	class GraphicObject {
	public:
		virtual void draw(float timestep) = 0;
	};

} // Namespace zombie.

#endif // GRAPHICOBJECT_H