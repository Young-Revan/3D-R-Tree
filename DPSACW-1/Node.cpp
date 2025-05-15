#include "Node.h"

Node::Node()
{
	MBP_ = Parallelepiped();
	data_ = nullptr;
	children_ = std::vector<Node*>();
	parent_ = nullptr;
}

Node::Node(std::string description)
{
	MBP_ = Parallelepiped();
	description_ = description;
	data_ = nullptr;
	children_ = std::vector<Node*>();
	parent_ = nullptr;
}

Node::Node(Parallelepiped const& MBP)
{
	MBP_ = MBP;
	data_ = nullptr;
	children_ = std::vector<Node*>();
	parent_ = nullptr;
}

Node::Node(std::string description, Parallelepiped const& MBP)
{
	MBP_ = MBP;
	description_ = description;
	data_ = nullptr;
	children_ = std::vector<Node*>();
	parent_ = nullptr;
}

Node::Node(std::string description, void* data, Parallelepiped const& MBP)
{
	MBP_ = MBP;
	description_ = description;
	data_ = data;
	children_ = std::vector<Node*>();
	parent_ = nullptr;
}

Node::Node(Node const& node)
{
	MBP_ = node.MBP_;
	description_ = node.description_;
	data_ = node.data_;
	children_ = node.children_;
	parent_ = node.parent_;
}

Node::~Node()
{
	for (int i = 0; i < children_.size(); i++)
	{
		delete children_[i];
	}
}

void Node::add(Node* newObject)
{
	children_.push_back(newObject);
	newObject->parent_ = this;
}

void Node::del(Node* object)
{
	std::vector<Node*> v;

	for (int i = 0; i < children_.size(); i++)
	{
		if (children_[i] != object)
		{
			v.push_back(children_[i]);
		}
	}
	children_ = v;
	object->breakBounds();
	delete object;
}

std::ostream& Node::show(std::ostream& out, std::string tab) const
{
	out << tab << description_ << '\n';
	out << tab << MBP_.getX() << '\t' << MBP_.getY() << '\t' << MBP_.getZ() << '\n';
	out << tab << MBP_.getOx() << '\t' << MBP_.getOy() << '\t' << MBP_.getOz() << '\n';
	out << tab << MBP_.getX2() << '\t' << MBP_.getY2() << '\t' << MBP_.getZ2() << '\n';

	if (children_.size() < 1) return out << '\n';

	out << tab << '\t' << "------" << '\n' << '\n';
	for (int i = 0; i < children_.size(); i++)
	{
		children_[i]->show(out, tab + '\t');
	}
	out << tab << "------";
	out << '\n';
	return out;
}

std::ostream& operator << (std::ostream& out, Node const& node)
{
	if (&node)
	{
		out << node.description_ << '\n';
		out << node.MBP_.getX() << '\t' << node.MBP_.getY() << '\t' << node.MBP_.getZ() << '\n';
		out << node.MBP_.getOx() << '\t' << node.MBP_.getOy() << '\t' << node.MBP_.getOz() << '\n';
		out << node.MBP_.getX2() << '\t' << node.MBP_.getY2() << '\t' << node.MBP_.getZ2() << '\n';
		return out;
	}
	else
	{
		return out << "Такого узла нет";
	}
}

std::ostream& operator << (std::ostream& out, Node const* node)
{
	if (node)
	{
		out << node->description_ << '\n';
		out << node->MBP_.getX() << '\t' << node->MBP_.getY() << '\t' << node->MBP_.getZ() << '\n';
		out << node->MBP_.getOx() << '\t' << node->MBP_.getOy() << '\t' << node->MBP_.getOz() << '\n';
		out << node->MBP_.getX2() << '\t' << node->MBP_.getY2() << '\t' << node->MBP_.getZ2() << '\n';
		return out;
	}
	else
	{
		return out << "Такого узла нет";
	}
}

int Node::distance(Point const& point) const
{
	if (MBP_.contains(point)) return 0;

	Point center = MBP_.location();

	double x = point.getX() - center.getX();
	double y = point.getY() - center.getY();
	double z = point.getZ() - center.getZ();

	double d = sqrt(x * x + y * y + z * z);

	return (int)d;
}

void Node::breakBounds()
{
	children_ = std::vector<Node*>();
}

std::pair<Node*, Node*> Node::splitByAxis(int axis) const
{
	std::pair<Node*, Node*> variant;

	// Сортируем потомков в узле по границам кубоидов на указанной оси
	// т.е. в этих массивах лежат индексы потомков в порядке следования по указанной оси по "левой" и "правой" границе
	std::vector<int> leftBorder = sortByAxis(axis, 0);
	std::vector<int> rightBorder = sortByAxis(axis, 1);

	// Формируем новые кубоиды в соответсвии с этим порядком

	variant.first = new Node(description_ + "-dl");
	variant.first->parent_ = parent_;

	// Формируем "левейший" узел по "левым" границам
	int n2 = children_.size() / 2;
	variant.first->MBP_ = children_[leftBorder[0]]->MBP_;
	variant.first->children_.push_back(children_[leftBorder[0]]);
#ifdef DEBUG
	TDRtree::operations++;
#endif // DEBUG
	for (int i = 1; i < n2; i++)
	{
		// Расширяем кубоид
		variant.first->MBP_ = variant.first->MBP_.extend(children_[leftBorder[i]]->MBP_);
		// Добавляем ему соответсвующих потомков
		variant.first->children_.push_back(children_[leftBorder[i]]);
#ifdef DEBUG
		TDRtree::operations++;
#endif // DEBUG
	}

	variant.second = new Node(description_ + "-dr");
	variant.second->parent_ = parent_;

	// Формируем "правейший" узел по "левым" границам - аналогично
	variant.second->MBP_ = children_[leftBorder[n2]]->MBP_;
	variant.second->children_.push_back(children_[leftBorder[n2]]);
#ifdef DEBUG
	TDRtree::operations++;
#endif // DEBUG
	for (int i = n2 + 1; i < children_.size(); i++)
	{
		variant.second->MBP_ = variant.second->MBP_.extend(children_[leftBorder[i]]->MBP_);
		variant.second->children_.push_back(children_[leftBorder[i]]);
#ifdef DEBUG
		TDRtree::operations++;
#endif // DEBUG
	}

	// Если дочерние узлы расположены "адекватно", то разделить можно только одним способом
	if (leftBorder == rightBorder) return variant;

	// ------------------------------------------------------------
	// 
	// Если узлы расположены через Жэ, то...

	std::pair<Node*, Node*> variant2;

	variant2.first = new Node(description_ + "-dl");
	variant2.first->parent_ = parent_;

	// Формируем "левейший" узел по "правым" границам
	n2 = children_.size() / 2;
	variant2.first->MBP_ = children_[rightBorder[0]]->MBP_;
	variant2.first->children_.push_back(children_[leftBorder[0]]);
#ifdef DEBUG
	TDRtree::operations++;
#endif // DEBUG
	for (int i = 1; i < n2; i++)
	{
		// Расширяем кубоид
		variant2.first->MBP_ = variant2.first->MBP_.extend(children_[rightBorder[i]]->MBP_);
		// Добавляем ему соответсвующих потомков
		variant2.first->children_.push_back(children_[rightBorder[i]]);
#ifdef DEBUG
		TDRtree::operations++;
#endif // DEBUG
	}

	variant2.second = new Node(description_ + "-dr");
	variant2.second->parent_ = parent_;

	// Формируем "правейший" узел по "правым" границам - аналогично
	variant2.second->MBP_ = children_[rightBorder[n2]]->MBP_;
	variant2.second->children_.push_back(children_[leftBorder[n2]]);
#ifdef DEBUG
	TDRtree::operations++;
#endif // DEBUG
	for (int i = n2 + 1; i < children_.size(); i++)
	{
		variant2.second->MBP_ = variant2.second->MBP_.extend(children_[rightBorder[i]]->MBP_);
		variant2.second->children_.push_back(children_[rightBorder[i]]);
#ifdef DEBUG
		TDRtree::operations++;
#endif // DEBUG
	}

	// Исследуем новые узлы на предмет пересечений друг с другом

	Parallelepiped v1 = variant.first->MBP_.intersection(variant.second->MBP_);
#ifdef DEBUG
	TDRtree::operations++;
#endif // DEBUG
	Parallelepiped v2 = variant2.first->MBP_.intersection(variant2.second->MBP_);
#ifdef DEBUG
	TDRtree::operations++;
#endif // DEBUG

	// Если первый вариант дал в итоге меньше пересечения, то возвращаем его
	if (v1 < v2)
	{
		variant2.first->breakBounds();
		variant2.second->breakBounds();
		delete variant2.first;
		delete variant2.second;

		return variant;
	}

	// То же со вторым
	if (v1 > v2)
	{
		variant.first->breakBounds();
		variant.second->breakBounds();
		delete variant.first;
		delete variant.second;

		return variant2;
	}

	// Если оба варианта дают одинаковое пересечение, выберем тот, что даёт наименьший в сумме объём

	int vol1 = variant.first->MBP_.volume() + variant.second->MBP_.volume();
#ifdef DEBUG
	TDRtree::operations++;
#endif // DEBUG
	int vol2 = variant2.first->MBP_.volume() + variant2.second->MBP_.volume();
#ifdef DEBUG
	TDRtree::operations++;
#endif // DEBUG

	if (vol1 <= vol2)
	{
		variant2.first->breakBounds();
		variant2.second->breakBounds();
		delete variant2.first;
		delete variant2.second;

		return variant;
	}
	else
	{
		variant.first->breakBounds();
		variant.second->breakBounds();
		delete variant.first;
		delete variant.second;

		return variant2;
	}
}

std::pair<Node*, Node*> Node::mergeByAxis(Node* node, int axis) const
{
	// Сортируем всех соседей в узле по границам кубоидов на указанной оси
	// т.е. в этих массивах лежат индексы потомков в порядке следования по указанной оси по "левой" и "правой" границе
	std::vector<int> leftBorder = sortByAxis(axis, 0);
	std::vector<int> rightBorder = sortByAxis(axis, 1);

	// Если порядок следования элементов одинаковый по любым границам, то...
	if (leftBorder == rightBorder)
	{
		return mergeNodeByAxis(leftBorder, node);
	}
	else
	{
		std::pair<Node*, Node*> left;
		std::pair<Node*, Node*> right;
		
		// В противном случае, придётся сравнивать оба варианта друг с другом
		left = mergeNodeByAxis(leftBorder, node);
		right = mergeNodeByAxis(rightBorder, node);

		return chooseOptimalMergin(left, right, node);
	}
}

std::pair<Node*, Node*> Node::mergeNodeByAxis(std::vector<int> numeration, Node* node) const
{
	// В парах будем хранить адреса
	// в first - узел "предок"
	// в second - новый расширенный узел
	
	std::pair<Node*, Node*> left;
	std::pair<Node*, Node*> right;

	// Находим индекс узла по данной нумерации
	int index = findChildIndex(numeration, node);

	// Если наш узел с "левого" края, то можно взять только тот, что справа
	if (index == 0)
	{
		left.first = children_[numeration[1]];
#ifdef DEBUG
		TDRtree::operations++;
#endif // DEBUG
		left.second = new Node(*left.first);
		left.second->MBP_ = left.second->MBP_.extend(node->MBP_);
		return left;
	}

	// Если наш узел с "правого" края, то можно взять только тот, что слева
	if (index == numeration.size() - 1)
	{
		left.first = children_[numeration[index - 1]];
#ifdef DEBUG
		TDRtree::operations++;
#endif // DEBUG
		left.second = new Node(*left.first);
		left.second->MBP_ = left.second->MBP_.extend(node->MBP_);
		return left;
	}

	// Если наш узел где-то в середине, то:

	// Берём левый узел
	left.first = children_[numeration[index - 1]];
#ifdef DEBUG
	TDRtree::operations++;
#endif // DEBUG
	left.second = new Node(*left.first);
	left.second->MBP_ = left.second->MBP_.extend(node->MBP_);

	// Берём правый узел узел
	right.first = children_[numeration[index + 1]];
#ifdef DEBUG
	TDRtree::operations++;
#endif // DEBUG
	right.second = new Node(*right.first);
	right.second->MBP_ = right.second->MBP_.extend(node->MBP_);

	return chooseOptimalMergin(left, right, node);
}

std::pair<Node*, Node*> Node::chooseOptimalMergin(std::pair<Node*, Node*> left, std::pair<Node*, Node*> right, Node* node) const
{
	int v1 = 0;
	int v2 = 0;

	// Соберём суммарный объём пересечений у обоих вариантов
	for (int i = 0; i < children_.size(); i++)
	{
		if (children_[i] != node)
		{
			if (children_[i] != left.first)
			{
				v1 += left.second->MBP_.intersection(children_[i]->MBP_).volume();
			}

			if (children_[i] != right.first)
			{
				v2 += right.second->MBP_.intersection(children_[i]->MBP_).volume();
			}
		}
#ifdef DEBUG
		TDRtree::operations++;
#endif // DEBUG
	}

	// Сравниваем разницу в пересечении
	if (v1 < v2)
	{
		right.second->breakBounds();
		delete right.second;
		return left;
	}

	if (v2 < v1)
	{
		left.second->breakBounds();
		delete left.second;
		return right;
	}

	// Если пересечение одинаковое, то смотрим разницу в наращивании объёма узла
	int sv1 = left.second->MBP_.volume() - left.first->MBP_.volume();
#ifdef DEBUG
	TDRtree::operations++;
#endif // DEBUG
	int sv2 = right.second->MBP_.volume() - right.first->MBP_.volume();
#ifdef DEBUG
	TDRtree::operations++;
#endif // DEBUG

	if (sv1 < sv1)
	{
		right.second->breakBounds();
		delete right.second;
		return left;
	}

	if (sv2 < sv1)
	{
		left.second->breakBounds();
		delete left.second;
		return right;
	}

	// Если и этот параметр одинаковый, то сравниваем абсолютный объём

#ifdef DEBUG
	TDRtree::operations++;
#endif // DEBUG
	if (left.second->MBP_.volume() <= right.second->MBP_.volume())
	{
		right.second->breakBounds();
		delete right.second;
		return left;
	}
	else
	{
		left.second->breakBounds();
		delete left.second;
		return right;
	}
}

int Node::findChildIndex(std::vector<int> numeration, Node* child) const
{
	for (int i = 0; i < children_.size(); i++)
	{
		if (children_[numeration[i]] == child)
		{
#ifdef DEBUG
			TDRtree::operations++;
#endif // DEBUG
			return i;
		}
	}
	return 0;
}

void Node::reduce()
{
	Parallelepiped p = children_[0]->MBP_;
	for (int i = 1; i < children_.size(); i++)
	{
		p = p.extend(children_[i]->MBP_);
#ifdef DEBUG
		TDRtree::operations++;
#endif // DEBUG
	}
	MBP_ = p;
}

std::vector<int> Node::sortByAxis(int axis, int border) const
{
	// Создаём массив, где будут хранится индексы дочерних узлов в порядке следования их указанных границ по указанной оси
	std::vector<int> borders = std::vector<int>(children_.size());

	//std::vector<Node*> sequence = std::vector<Node*>(children_.size());

	// Предположим, что объекты стоят в том порядке, в котором они пронумерованы
	for (int i = 0; i < children_.size(); i++)
	{
		borders[i] = i;
	}

	// Сортируем по указанной границе и оси
	for (int i = 0; i < children_.size() - 1; i++)
	{
		int min = i;
		for (int j = i + 1; j < children_.size(); j++)
		{
			// Если указанная граница первого объекта находится "правее" границы второго (т.е. больше), то
			if (children_[borders[i]]->MBP_.get(border, axis) > children_[borders[j]]->MBP_.get(border, axis))
			{
				// "меняем их местами"
				int tmp = borders[i];
				borders[i] = borders[j];
				borders[j] = tmp;
#ifdef DEBUG
				TDRtree::operations += 2;
#endif // DEBUG
			}
		}
	}

	return borders;
}

bool Node::isLeaf() const
{
	return children_.size() == 0;
}

bool Node::childrenAreLeafs() const
{
	for (int i = 0; i < children_.size(); i++)
	{
#ifdef DEBUG
		TDRtree::operations++;
#endif // DEBUG
		if (!children_[i]->isLeaf()) return false;
	}
	return true;
}

Node* Node::chooseChild(Node* newObject)
{
	std::vector<Parallelepiped> plps = std::vector<Parallelepiped>(children_.size());
	std::vector<int> volumes = std::vector<int>(children_.size());
	std::vector<int> intersections = std::vector<int>(children_.size());

	// Создаём массив новых расширенных кубоидов, тут же рассчитываем, на сколько увеличился объём расширенного кубоида
	for (int i = 0; i < children_.size(); i++)
	{
		plps[i] = children_[i]->MBP_.extend(newObject->MBP_);
		volumes[i] = plps[i].volume();
#ifdef DEBUG
		TDRtree::operations++;
#endif // DEBUG

		// Теперь рассматриваем пересечения нового кубоида с остальными
		intersections[i] = 0;
		for (int j = 0; j < children_.size(); j++)
		{
			if (j != i)
			{
				// Вычисляем кубоид на пересечении, получаем его объём
				intersections[i] += plps[i].intersection(children_[j]->MBP_).volume();
#ifdef DEBUG
				TDRtree::operations++;
#endif // DEBUG
			}
		}
	}

	int optimal = 0;
	int last = 0;

	// Ищем тот кубоид, что при увеличении выдаёт минимальное пересечение
	for (int i = 1; i < children_.size(); i++)
	{
		if (intersections[i] <= intersections[i - 1])
		{
			last = optimal;
			optimal = i;
		}
	}

	// Если таковой есть, то возвращаем данный узел, предварительно его увеличив
	if ((intersections[optimal] > intersections[last] && optimal != last) || optimal == last)
	{
		children_[optimal]->MBP_ = children_[optimal]->MBP_.extend(newObject->MBP_);
#ifdef DEBUG
		TDRtree::operations++;
#endif // DEBUG
		return children_[optimal];
	}

	std::vector<int> indexes;

	// Записываем все узлы с одинаковым расширением
	indexes.push_back(optimal);
	for (int i = 0; i < children_.size(); i++)
	{
		if (i != optimal && intersections[i] == intersections[optimal]) indexes.push_back(i);
	}

	// Сравниваем их объём
	for (int i = 1; i < indexes.size(); i++)
	{
		if (volumes[indexes[i]] < volumes[indexes[i - 1]]) optimal = indexes[i];
	}

	children_[optimal]->MBP_ = children_[optimal]->MBP_.extend(newObject->MBP_);
#ifdef DEBUG
	TDRtree::operations++;
#endif // DEBUG
	return children_[optimal];
}