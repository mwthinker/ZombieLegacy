#ifndef QUADTREE_H
#define QUADTREE_H

#include <array>
#include <vector>

template <class T, class Container = std::vector<T>>
class QuadNode {
public:
	QuadNode(int level, int maxLevel) : level_(level), maxLevel_(maxLevel) {
		if (level < maxLevel) {
			children_[0] = new QuadNode(level+1,maxLevel);
			children_[1] = new QuadNode(level+1,maxLevel);
			children_[2] = new QuadNode(level+1,maxLevel);
			children_[3] = new QuadNode(level+1,maxLevel);
		} else {
			children_[0] = nullptr;
			children_[1] = nullptr;
			children_[2] = nullptr;
			children_[3] = nullptr;
		}
	}

	~QuadNode() {
		delete children_[0];
		delete children_[1];
		delete children_[2];
		delete children_[3];
	}

	void addObject(double x, double y, double width, double height, T object) {
		if (level_ == maxLevel_) {
			objects_.push_back(object);
		} else {
			if (x < 0.5 && x + width < 0.5) {
				if (y < 0.5 && y + height < 0.5) {
					children_[SW]->addObject(x*2, y*2, width*2, height*2, object);
				} else if (y >= 0.5) {
					children_[NW]->addObject(x*2, (y-0.5)*2, width*2, height*2, object);
				} else {
					objects_.push_back(object);
				}
			} else if (x >= 0.5) {
				if (y < 0.5 && y + height < 0.5) {
					children_[SE]->addObject((x-0.5)*2, y*2, width*2, height*2, object);
				} else if (y >= 0.5) {
					children_[NE]->addObject((x-0.5)*2, (y-0.5)*2, width*2, height*2, object);
				} else {
					objects_.push_back(object);
				}
			} else {
				objects_.push_back(object);
			}
		}
	}

	Container getObjectsAt(double x, double y, double width, double height) const {
		if (level_ == maxLevel_) {
			return objects_;
		} else {
			std::vector<T> returnObjects = objects_;
			std::vector<T> tmp;

			bool sw = false, se = false, nw = false, ne = false;
			insideSquare(x,y,sw,se,nw,ne);
			insideSquare(x+width,y,sw,se,nw,ne);
			insideSquare(x+width,y+height,sw,se,nw,ne);
			insideSquare(x,y+height,sw,se,nw,ne);

			if (sw) {
				tmp = children_[SW]->getObjectsAt(x*2, y*2, width*2, height*2);
				returnObjects.insert(returnObjects.end(), tmp.begin(), tmp.end());
			}
			if (se) {
				tmp = children_[SE]->getObjectsAt((x-0.5)*2, y*2, width*2, height*2);
				returnObjects.insert(returnObjects.end(), tmp.begin(), tmp.end());
			}
			if (nw) {
				tmp = children_[NW]->getObjectsAt(x*2, (y-0.5)*2, width*2, height*2);
				returnObjects.insert(returnObjects.end(), tmp.begin(), tmp.end());
			}
			if (ne) {
				tmp = children_[NE]->getObjectsAt((x-0.5)*2, (y-0.5)*2, width*2, height*2);
				returnObjects.insert(returnObjects.end(), tmp.begin(), tmp.end());
			}
			
			returnObjects.insert(returnObjects.end(), objects_.begin(), objects_.end());
			return returnObjects;
		}
	}

	Container getObjectsAt(double x, double y, double radius) const {
		return getObjectsAt(x, y, radius, radius);
	}
private:
	static void insideSquare(double x, double y, bool& sw, bool& se, bool& nw, bool& ne) {
		if (x < 0.5) {
			if (y < 0.5) {
				sw = true;
			} else {
				nw = true;
			}
		} else {
			if (y < 0.5) {
				se = true;
			} else {
				ne = true;
			}
		}
	}

	// | 1 3 |     <=> | 01 11 |
	// | 0 2 |_10      | 00 10 |_2
	// A number representing a child. Left bit in each pair represent x-coord and right y-coord.
	enum Node {SW=0, NW=1,SE=2, NE=3};

	std::array<QuadNode*,4> children_;
	std::vector<T> objects_;

	int level_, maxLevel_;
};

template <class T, class Container = std::vector<T>>
class Quadtree {
public:
	Quadtree() : x_(0), y_(0), width_(1), height_(1), maxLevel_(4), head_(0, maxLevel_) {
	}
	
	Quadtree(double x, double y, double width, double height, int maxLevel) : x_(x), y_(y), width_(width), height_(height), maxLevel_(maxLevel), head_(0, maxLevel) {
	}

	void add(T ob, double x, double y, double width, double height) {
		objects_.push_back(ob);
		head_.addObject((x - x_)/width_,(y - y_)/height_,width/width_,height/height_,ob);
	}

	Container getObjectsAt(double x, double y, double width, double height) {
		return head_.getObjectsAt((x - x_)/width_,(y - y_)/height_,width/width_,height/height_);
	}

	Container getObjectsAt(double x, double y, double radius) {
		return head_.getObjectsAt((x - x_)/width_,(y - y_)/height_,radius);
	}

	const Container& getElements() const {
		return objects_;
	}

	int getMaxLevel() const {
		return maxLevel_;
	}
private:
	double x_, y_, width_, height_;
	int maxLevel_;

	Container objects_;
	QuadNode<T> head_;
};

#endif // QUADTREE_H