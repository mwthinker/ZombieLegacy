#ifndef ACTIONHANDLER_H
#define ACTIONHANDLER_H

namespace zombie {

	class Unit;
	class Car;

	class ActionHandler {
	public:
		virtual void unitEvent(Unit* unit, int eventType) = 0;

		virtual void carEvent(Car* unit, int eventType) = 0;

	protected:
		~ActionHandler() {
		}
	};

} // Namespace zombie.

#endif // ACTIONHANDLER_H
