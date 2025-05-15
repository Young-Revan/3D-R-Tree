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

	// ��������� ����� ������ � ������
	void add(std::string description, void* data, Parallelepiped const& MBP);

	// ���������� ������ �����, ������� �������� ������ �����
	std::vector<Node*> pointSearch(Point const& point) const;

	// ���������� ������ �����, ������� ������������� ������ ��������
	std::vector<Node*> rangeSearch(Parallelepiped const& parallelepiped) const;

	// ����� ��������� �������
	std::vector<Node*> closestNeighbours(Point const& point, int max) const;

	// ����� ����������� ���� �� �������� � �������
	Node* find(std::string description, Parallelepiped const& MBP) const;

	// �������� ��������� ���� �� ����� ���������
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

	// ���������� ��������� ���� �� ��� ������ ����
	void split(Node* node);

	// ���������� ����������� ���������� ���� � ������ �����
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

