#pragma once

#include "Parallelepiped.h"
#include <iostream>
#include <string>
#include <map>

class Node
{
	friend class TDRtree;
public:
	Node();
	Node(std::string description);
	Node(Parallelepiped const& MBP);
	Node(std::string description, Parallelepiped const&  MBP);
	Node(std::string description, void* data, Parallelepiped const& MBP);
	Node(Node const& node);
	~Node();

	void add(Node* newObject);
	void del(Node* object);

	std::ostream& show(std::ostream& out, std::string tab) const;
	friend std::ostream& operator << (std::ostream& out, Node const& node);
	friend std::ostream& operator << (std::ostream& out, Node const* node);

	int distance(Point const& point) const;

private:

	// ������ ����� "��������" ��� ����� � ��������� � �������, �� ������ ��!
	void breakBounds();

	// ���������� ������ �������� ���������, ��������������� �� �������� ���
	std::vector<int> sortByAxis(int axis, int border) const;

	// ���������� ��� ����, ���������� � ���������� ������� ���� �� �������� ���
	std::pair<Node*, Node*> splitByAxis(int axis) const;

	std::pair<Node*, Node*> mergeByAxis(Node* node, int axis) const;

	std::pair<Node*, Node*> mergeNodeByAxis(std::vector<int> numeration, Node* node) const;

	std::pair<Node*, Node*> chooseOptimalMergin(std::pair<Node*, Node*> left, std::pair<Node*, Node*> right, Node* node) const;

	int findChildIndex(std::vector<int> numeration, Node* child) const;

	void reduce();

	bool isLeaf() const;
	bool childrenAreLeafs() const;

	Node* chooseChild(Node* newObject);

private:
	Parallelepiped MBP_;
	std::string description_;
	void* data_;
	std::vector<Node*> children_;
	Node* parent_;
};