// marcys dator = virus
#ifndef PATHFINDER_H
#define PATHFINDER_H

#include <vector>

class Node {
public:
	Node() {}
	Node(int f) {
		f_ = f;
	}	
	
	inline Node const* parent() const {
		return parent_;
	}
	inline int row() const {
		return row_;
	}
	inline int colon() const {
		return colon_;
	}
private:
	friend bool compareNodes(const Node*, const Node*);
	friend class PathFinder;
	Node* parent_;
	int row_, colon_;
	bool obstacle_;	
	int f_, h_, g_;
	unsigned int id_;
};

bool compareNodes(const Node*, const Node*);

class PathFinder {
public:
	PathFinder(int nbrOfRows, int nbrOfColons, double heightWorld, double widthWorld);
	~PathFinder();
	
	Node const* findPath(double x, double y, double xDestination, double yDestination);

	void setObstacle(int row, int colon, bool obstacle);
	bool getObstacle(int row, int colon) const;

	double xPos(int colon) const;
	double yPos(int row) const;
	int row(double y) const;
	int colon(double x) const;

	double worldWidth() const;
	double worldHeight() const;
	int nbrOfRows() const;
	int nbrOfColons() const;
private:
	Node const* findPath(int row, int colon, int rowDestination, int colonDestination);
	void addSurroundingNodes(Node* node, const Node* end);
	bool insideClosedList(const Node* node) const;
	bool insideOpenList(const Node* node) const;
	
	Node*** map_;
	std::vector<Node*> openList_;
	std::vector<Node*> closeList_;
	double heightWorld_, widthWorld_;
	int nbrOfRows_, nbrOfColons_;
};

#endif // PATHFINDER_H