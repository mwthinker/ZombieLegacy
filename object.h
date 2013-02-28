#ifndef OBJECT_H
#define OBJECT_H

namespace zombie {

	// Represent a object living inside the "zombie world".
	class Object {
	public:
		Object(int id);
		virtual ~Object();

		// Returns true if the position (x,y) is inside the object.
		virtual bool isInside(double x, double y) const = 0;

		virtual double healthPoints() const = 0;
		virtual void updateHealthPoint(double deltaLife) = 0;
		virtual bool isDead() const = 0;
		int id() const;
	private:
		int id_;
		static int lastAddedId_;
	};

} // namespace zombie

#endif // OBJECT_H