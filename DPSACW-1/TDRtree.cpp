#include "TDRtree.h"

int TDRtree::operations = 0;

TDRtree::TDRtree()
{
	root_ = new Node("root");

#ifdef DEBUG
	N_ = 0;
#endif // DEBUG
}

TDRtree::~TDRtree()
{
	delete root_;
}

void TDRtree::add(std::string description, void* data, Parallelepiped const& MBP)
{
	// ������ ����� ���� �� ���� �������� ����
	Node* newObject = new Node(description, data, MBP);

	// ���� ����, � ������� ���� �������� ������ ����
	Node* node = choose(newObject);

	// �� ������ ����� ��� ���� ���� ���������, �� ������ ��� ������� ������ ����� � ������

	// �������
	node->add(newObject);

#ifdef DEBUG
	N_++;
#endif // DEBUG

	// ���������, ����� � ����, � ������� �������� ����� ������, ���� �� ������ 5 �����
	normalize(node);
}

std::ostream& operator << (std::ostream& out, TDRtree const& tree)
{
#ifdef DEBUG
	out << "���������� �������: " << tree.N_ << '\n';
#endif // DEBUG
	return tree.root_->show(out, "");
}

void TDRtree::replace(Node* old, Node* n1, Node* n2)
{
	// ���� ������ ���� ��� ������, ��
	if (old == root_)
	{
		Node* newRoot = new Node("root");
		newRoot->MBP_ = newRoot->MBP_.extend(n1->MBP_);
		newRoot->MBP_ = newRoot->MBP_.extend(n2->MBP_);
		newRoot->add(n1);
		newRoot->add(n2);
		redirection(old, n1);
		redirection(old, n2);
		root_->breakBounds();
		delete root_;
		root_ = newRoot;
		return;
	}

	// ����� ������� ������� ���� �� ������ � ������������ ����
	redirection(old, n1);
	redirection(old, n2);
	Node* parent = old->parent_;
	parent->del(old);

	// � ��������� ����� ���� � ������������ ������
	parent->add(n1);
	parent->add(n2);
}

void TDRtree::split(Node* node)
{
	// ������ ������ ��� ����� ����� (���������� ������ ����)
	std::vector<std::pair<Node*, Node*>> variants;

	// ������ �������� ���� ��� �� ������ ���

	// ������� � �������� �� ��� Ox
	variants.push_back(node->splitByAxis(0));

	// ������� � �������� �� ��� Oy
	variants.push_back(node->splitByAxis(1));

	// ������� � �������� �� ��� Oz
	variants.push_back(node->splitByAxis(2));

	// ������ �� ���� �������� ����� ����� � ������� �������� �����������

	std::vector<Parallelepiped> intersections = std::vector<Parallelepiped>(3);
	std::vector<int> volumes = std::vector<int>(3);

	for (int i = 0; i < variants.size(); i++)
	{
		// ��������� ������ ������� �� �����������
		intersections[i] = variants[i].first->MBP_.intersection(variants[i].second->MBP_);
#ifdef DEBUG
		TDRtree::operations++;
#endif // DEBUG
		volumes[i] = intersections[i].volume();
	}

	// ���� ������ �������� ������ ����������� �����������
	int min = 0;
	int last = 0;
	for (int i = 1; i < variants.size(); i++)
	{
		// ���� ��������� ������� ������ ��� ����� �����������, ��
		if (volumes[i] <= volumes[i - 1])
		{
			last = min;
			min = i;
		}
	}

	// ���� ������� � ���������� ������� ���� ������������, ��:
	if (min != last && volumes[min] < volumes[last])
	{
		// ������� ��� ������ ���� �� ���� ������ � ������� ��
		for (int i = 0; i < variants.size(); i++)
		{
			if (i != min)
			{
				variants[i].first->breakBounds();
				variants[i].second->breakBounds();
				delete variants[i].first;
				delete variants[i].second;
			}
		}

		// ��������� ���� � �������� � ������ � ������� ������ ���� �� ����
		replace(node, variants[min].first, variants[min].second);
		return;
	}

	// ���� ��������� � ���������� ������� ���������, ��
	// ���� ��� ��������

	std::vector<int> im;	// ������� ��������� � ���������� ������� �����������
	std::vector<int> v;		// �������������� ������ ���� ��������
	for (int i = 0; i < variants.size(); i++)
	{
		if (volumes[i] == volumes[min])
		{
			im.push_back(i);
		}
	}

	// ���� ����� ������� ��� ������� ��������
	for (int i = 0; i < im.size(); i++)
	{
		// ������������ ����� �������
		v.push_back(variants[im[i]].first->MBP_.volume() + variants[im[i]].second->MBP_.volume());
#ifdef DEBUG
		TDRtree::operations++;
#endif // DEBUG
	}

	// ����� ���� ������� ����������� �����
	min = 0;
	for (int i = 1; i < im.size(); i++)
	{
		if (v[min] > v[i]) min = i;
	}

	// ������� ��� ������ ���� �� ���� ������ � ������� ��
	for (int i = 0; i < variants.size(); i++)
	{
		if (i != im[min])
		{
			variants[i].first->breakBounds();
			variants[i].second->breakBounds();
			delete variants[i].first;
			delete variants[i].second;
		}
	}

	// ��������� ���� � �������� � ������ � ������� ������ ���� �� ����
	replace(node, variants[im[min]].first, variants[im[min]].second);
}

void TDRtree::merge(Node* node)
{
	Node* parent = node->parent_;

	std::vector<std::pair<Node*, Node*>> variants;
	variants.push_back(parent->mergeByAxis(node, 0));
	variants.push_back(parent->mergeByAxis(node, 1));
	variants.push_back(parent->mergeByAxis(node, 2));

	// ������ � ��� ���� ��� �������� �������.
	// ���� ������� ����.
	// � first ����� ����, ������� �������� node.
	// � second ����� ����, ��� ����� "��� �����������" node
	// ���������� second �� ����� children ����� node � first

	// ����� ��������� � ������ �����������
	std::vector<int> intersections = std::vector<int>(variants.size());

	// ������� ����� ��������� ����� �����������
	for (int i = 0; i < parent->children_.size(); i++)
	{
		if (parent->children_[i] != node)
		{
			if (parent->children_[i] != variants[0].first)
			{
				intersections[0] += variants[0].second->MBP_.intersection(parent->children_[i]->MBP_).volume();
			}

			if (parent->children_[i] != variants[1].first)
			{
				intersections[1] += variants[1].second->MBP_.intersection(parent->children_[i]->MBP_).volume();
			}

			if (parent->children_[i] != variants[2].first)
			{
				intersections[2] += variants[2].second->MBP_.intersection(parent->children_[i]->MBP_).volume();
			}
		}
#ifdef DEBUG
		TDRtree::operations++;
#endif // DEBUG
	}

	// ����� ������� ��
	int min = 0;
	int last = 0;
	for (int i = 1; i < intersections.size(); i++)
	{
		if (intersections[i] <= intersections[min])
		{
			last = min;
			min = i;
		}
	}

	// ���� � ��� ������ ���� ���������� �������, �� �������� ������ ���
	if (last != min && intersections[min] < intersections[last])
	{
		for (int i = 1; i < variants.size(); i++)
		{
			if (i != min)
			{
				// ������� ������ �� ��� ��������� ������
				variants[i].second->breakBounds();
				delete variants[i].second;
			}
		}

		// ������������ ��� ���� �� ���������� ���� � ������ ����
		for (int i = 0; i < node->children_.size(); i++)
		{
			variants[min].first->add(node->children_[i]);
		}
		// ������ ������ �� �����
		variants[min].first->MBP_ = variants[min].second->MBP_;
		// ������� ��� �������� ����
		variants[min].second->breakBounds();
		delete variants[min].second;
		// ������� ������ ����
		parent->del(node);
	}

	// ���� ���������� ��������� ���������, �� �� ���� ��������
	std::vector<int> mins;

	for (int i = 0; i < intersections.size(); i++)
	{
		if (intersections[i] == intersections[min])
		{
			mins.push_back(i);
		}
	}

	// �� ��� ������� ��, ������� ���� ���������� ������� � ������
	std::vector<int> sv = std::vector<int>(mins.size());

	// ��������� ��� �������
	for (int i = 0; i < mins.size(); i++)
	{
		sv[i] = variants[mins[i]].second->MBP_.volume() - variants[mins[i]].first->MBP_.volume();
#ifdef DEBUG
		TDRtree::operations++;
#endif // DEBUG
	}

	// ������� �
	min = mins[0];
	for (int i = 0; i < mins.size(); i++)
	{
		if (sv[mins[i]] < sv[min]) min = sv[mins[i]];
	}

	// ��������� ������� ���������
	for (int i = 1; i < variants.size(); i++)
	{
		if (i != min)
		{
			// ������� ������ �� ��� ��������� ������
			variants[i].second->breakBounds();
			delete variants[i].second;
		}
	}

	// ������������ ��� ���� �� ���������� ���� � ������ ����
	for (int i = 0; i < node->children_.size(); i++)
	{
		variants[min].first->add(node->children_[i]);
	}
	// ������ ������ �� �����
	variants[min].first->MBP_ = variants[min].second->MBP_;
	// ������� ��� �������� ����
	variants[min].second->breakBounds();
	delete variants[min].second;
	// ������� ������ ����
	parent->del(node);
}

void TDRtree::normalize(Node* node)
{
	// ��������� ���� (���� ��������)
	node->reduce();
	
	// ���� ���������� ��������� � ���� �����������
	if (node->children_.size() > CHILDREN_MAX_COUNT)
	{
		// �� ����� ���� �� ��� �����
		Node* parent = node->parent_;
		split(node);
		if (parent) normalize(parent);
		return;
	}

	// ���� ���������� ��������� � ���� ������� ����
	if (node->children_.size() < CHILDREN_MIN_COUNT)
	{
		// � ���� �� �������� ������
		if (node == root_) return;

		// �� ��������� ��� � ������ �����
		Node* parent = node->parent_;
		merge(node);
		if (parent) normalize(parent);
		return;
	}
}

Node* TDRtree::choose(Node* newObject)
{
	// ���� ������ ����, �� ���������� ���
	//if (root_->isLeaf()) return root_;
	// ����� ���� ����� ��� ��������
	return chooseNode(root_, newObject);
}

Node* TDRtree::chooseNode(Node* node, Node* newObject)
{
#ifdef DEBUG
	TDRtree::operations++;
#endif // DEBUG
	// ���� � ������� ���� ���� ������ ������, ��� ���� ���� �������� ���� ������ ������, ���� ������ � ��������, �� ���������� ���� ����
	if (node->childrenAreLeafs() || (node == root_ && root_->isLeaf()))
	{
		// ��������������, ���� ������� ���������, ���� �����
		node->MBP_ = node->MBP_.extend(newObject->MBP_);
		return node;
	}

	// �������� �������� ���������� �������� ���� � ���������� ���
	Node* N = node->chooseChild(newObject);
	return chooseNode(N, newObject);
}

std::vector<Node*> TDRtree::pointSearch(Point const& point) const
{
	std::stack<Node*> s;
	cluster(point, root_, &s);

	std::vector<Node*> nodes;
	while (!s.empty())
	{
		nodes.push_back(s.top());
		s.pop();
	}
	return nodes;
}

std::vector<Node*> TDRtree::rangeSearch(Parallelepiped const& parallelepiped) const
{
	std::stack<Node*> s;
	cluster(parallelepiped, root_, &s);

	std::vector<Node*> nodes;
	while (!s.empty())
	{
		nodes.push_back(s.top());
		s.pop();
	}
	return nodes;
}

std::vector<Node*> TDRtree::closestNeighbours(Point const& point, int max) const
{
	std::vector<Node*> nodes;
	max -= 1;
	int count = 0;
	cluster(point, root_, &nodes, &max, &count);
	
	return nodes;
}

Node* TDRtree::find(std::string description, Parallelepiped const& MBP) const
{
	return cluster(root_, description, MBP);
}

void TDRtree::del(std::string description, Parallelepiped const& MBP)
{
	Node* node = find(description, MBP);

	if (node == root_) return;

	if (node)
	{
		del(node);
	}
}

void TDRtree::cluster(Point const& point, Node* node, std::stack<Node*>* s) const
{
#ifdef DEBUG
	TDRtree::operations++;
#endif // DEBUG
	// ���� ������ ���� �������� �����, �� ��������� ��� � ������, ������������� ��� ��������
	if (node->MBP_.contains(point))
	{
		s->push(node);
		// ���� ���� �������� ��������, �� ��������� ������
		if (node->isLeaf()) return;

		// ���� �� ����� �������� ����, ���������� ��� �����, �� ������������� � ��� ��� ��, ��� � ���� ����
		for (int i = 0; i < node->children_.size(); i++)
		{
#ifdef DEBUG
			TDRtree::operations++;
#endif // DEBUG
			cluster(point, node->children_[i], s);
		}
	}
}

void TDRtree::cluster(Parallelepiped const& parallelepiped, Node* node, std::stack<Node*>* s) const
{
#ifdef DEBUG
	TDRtree::operations++;
#endif // DEBUG
	// ���� ������ ���� ���������� �������, �� ��������� ��� � ������, ������������� ��� ��������
	if (node->MBP_.overlapping(parallelepiped))
	{
		s->push(node);
		// ���� ���� �������� ��������, �� ��������� ������
		if (node->isLeaf()) return;

		// ���� �� ����� �������� ����, ������������ �������� �������, �� ������������� � ��� ��� ��, ��� � ���� ����
		for (int i = 0; i < node->children_.size(); i++)
		{
#ifdef DEBUG
			TDRtree::operations++;
#endif // DEBUG
			cluster(parallelepiped, node->children_[i], s);
		}
	}
}

void TDRtree::cluster(Point const& point, Node* node, std::vector<Node*>* nodes, int* max, int* count) const
{
#ifdef DEBUG
	TDRtree::operations++;
#endif // DEBUG
	// ���� �� ��� ����� �������, ������� ������, �� ������� �� ������
	if (*max <= *count) return;

	// ���� ���� �������� ������, �� ��������� ��� � ������, ����������� �������, � ������� �� ������
	if (node->isLeaf())
	{
		nodes->push_back(node);
		(*count)++;
		return;
	}

	// ���� � ���� ���� �������, ����� ���� ����������� �� ����, ����� � ������ ��������
	// ��� ����� ����������� �������� � ������� ���������� ��������� �� ���

	std::vector<int> distances = std::vector<int>(node->children_.size());
	std::vector<int> indexes = std::vector<int>(node->children_.size());

	// ������� ��� ������� ��� ��������� � ��� �������������� ��������
	for (int i = 0; i < node->children_.size(); i++)
	{
		distances[i] = node->children_[i]->distance(point);
#ifdef DEBUG
		TDRtree::operations++;
#endif // DEBUG
		indexes[i] = i;
	}

	// ����������
	for (int i = 0; i < indexes.size() - 1; i++)
	{
		for (int j = i + 1; j < indexes.size(); j++)
		{
			// ���� ��������� ���������� �������� ������ �������� ������������, �� ������ �� �������
			if (distances[indexes[j]] < distances[indexes[i]])
			{
				int tmp = indexes[j];
				indexes[j] = indexes[i];
				indexes[i] = tmp;
			}
		}
	}

	// ������ ����������� �� ��������������� ���������:
	for (int i = 0; i < indexes.size(); i++)
	{
		// ������������� ��������� �������
		cluster(point, node->children_[indexes[i]], nodes, max, count);
#ifdef DEBUG
		TDRtree::operations++;
#endif // DEBUG

		// ���� �� ��� ����� �������, ������� ������, �� ������� �� ������
		if (*max <= *count) return;
	}

	// ���� ��� �������� ���� ���� ������������, � ��� ����� ������, �� ��������� ���� � ������ �������
	nodes->push_back(node);
	(*count)++;
}

Node* TDRtree::cluster(Node* node, std::string description, Parallelepiped const& MBP) const
{
#ifdef DEBUG
	TDRtree::operations++;
#endif // DEBUG
	if (node->description_ == description && node->MBP_ == MBP) return node;

	for (int i = 0; i < node->children_.size(); i++)
	{
#ifdef DEBUG
		TDRtree::operations++;
#endif // DEBUG
		if (node->children_[i]->MBP_.contains(MBP)) return cluster(node->children_[i], description, MBP);
	}

	return nullptr;
}

void TDRtree::redirection(Node* was, Node* now)
{
	for (int i = 0; i < was->children_.size(); i++)
	{
		for (int j = 0; j < now->children_.size(); j++)
		{
			if (was->children_[i] == now->children_[j]) now->children_[j]->parent_ = now;
		}
	}
}

void TDRtree::del(Node* node)
{
	Node* parent = node->parent_;
	std::vector<Node*> v;
	for (int i = 0; i < parent->children_.size(); i++)
	{
		if (parent->children_[i] != node)
		{
			v.push_back(parent->children_[i]);
		}
	}
	parent->children_ = v;
#ifdef DEBUG
	N_--;
#endif // DEBUG
	delete node;
	normalize(parent);
}