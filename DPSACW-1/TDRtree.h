#pragma once

#define DEBUG

#include "Node.h"
#include <stack>

#define CHILDREN_MAX_COUNT 5
#define CHILDREN_MIN_COUNT 2

class TDRtree
{
	friend class Node;
private:
	

public:
	TDRtree();
	~TDRtree();

	// Добавляет новый объект в дерево
	void add(std::string description, void* data, Parallelepiped const& MBP);

	// Возвращает массив узлов, которые содержат данную точку
	std::vector<Node*> pointSearch(Point const& point) const;

	// Возвращает массив узлов, которые перекрываются данной областью
	std::vector<Node*> rangeSearch(Parallelepiped const& parallelepiped) const;

	// Поиск ближайших соседей
	std::vector<Node*> closestNeighbours(Point const& point, int max) const;

	// Поиск конкретного узла по названию и региону
	Node* find(std::string description, Parallelepiped const& MBP) const;

	// Удаление заданного узла со всеми потомками
	void del(std::string description, Parallelepiped const& MBP);
	
	friend std::ostream& operator << (std::ostream& out, TDRtree const&);

private:
	void replace(Node* old, Node* n1, Node* n2);

	Node* choose(Node* newObject);
	Node* chooseNode(Node* node, Node* newObject);

	Node* cluster(Node* node, std::string description, Parallelepiped const& MBP) const;
	void cluster(Point const& point, Node* node, std::stack<Node*>* s) const;
	void cluster(Parallelepiped const& parallelepiped, Node* node, std::stack<Node*>* s) const;
	void cluster(Point const& point, Node* node, std::vector<Node*>* nodes, int* max, int* count) const;

	// Производит разбиение узла на два других узла
	void split(Node* node);

	// Производит оптимальное сращивание узла с другим узлом
	void merge(Node* node);

	void normalize(Node* node);

	void del(Node* node);

	void redirection(Node* was, Node* now);

private:
	Node* root_;

#ifdef DEBUG
	int N_;

public:
	static int operations;
#endif // DEBUG

};

