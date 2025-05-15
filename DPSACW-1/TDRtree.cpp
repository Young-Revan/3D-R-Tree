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
	// Создаём новый лист на базе введённой инфы
	Node* newObject = new Node(description, data, MBP);

	// Ищем узел, в который надо вставить данный лист
	Node* node = choose(newObject);

	// На данном этапе все узлы были расширены, всё готово для вставки нового листа в дерево

	// Вставка
	node->add(newObject);

#ifdef DEBUG
	N_++;
#endif // DEBUG

	// Проверяем, чтобы в узле, в который вставили новый объект, было не больше 5 узлов
	normalize(node);
}

std::ostream& operator << (std::ostream& out, TDRtree const& tree)
{
#ifdef DEBUG
	out << "Количество листьев: " << tree.N_ << '\n';
#endif // DEBUG
	return tree.root_->show(out, "");
}

void TDRtree::replace(Node* old, Node* n1, Node* n2)
{
	// Если старый узел был корнем, то
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

	// Иначе удаляем текущий узел из списка в родительском узле
	redirection(old, n1);
	redirection(old, n2);
	Node* parent = old->parent_;
	parent->del(old);

	// И добавляем новые узлы в родительский список
	parent->add(n1);
	parent->add(n2);
}

void TDRtree::split(Node* node)
{
	// Создаём массив пар новых узлов (разделённый старый узел)
	std::vector<std::pair<Node*, Node*>> variants;

	// Создаём варианты этих пар по каждой оси

	// Вариант с делением по оси Ox
	variants.push_back(node->splitByAxis(0));

	// Вариант с делением по оси Oy
	variants.push_back(node->splitByAxis(1));

	// Вариант с делением по оси Oz
	variants.push_back(node->splitByAxis(2));

	// Теперь их надо сравнить между собой и выбрать наиболее оптимальный

	std::vector<Parallelepiped> intersections = std::vector<Parallelepiped>(3);
	std::vector<int> volumes = std::vector<int>(3);

	for (int i = 0; i < variants.size(); i++)
	{
		// Проверяем каждый вариант на пересечения
		intersections[i] = variants[i].first->MBP_.intersection(variants[i].second->MBP_);
#ifdef DEBUG
		TDRtree::operations++;
#endif // DEBUG
		volumes[i] = intersections[i].volume();
	}

	// Ищем индекс варианта дающий минимальное пересечение
	int min = 0;
	int last = 0;
	for (int i = 1; i < variants.size(); i++)
	{
		// Если очередной вариант меньше даёт объём пересечений, то
		if (volumes[i] <= volumes[i - 1])
		{
			last = min;
			min = i;
		}
	}

	// Если вариант с наименьшим объёмом один единственный, то:
	if (min != last && volumes[min] < volumes[last])
	{
		// Очищаем все прочие узлы от всех связей и удаляем их
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

		// Добавляем узлы к родителю в список и удаляем старый узел из него
		replace(node, variants[min].first, variants[min].second);
		return;
	}

	// Если вариантов с наименьшим объёмом несколько, то
	// Ищем эти варианты

	std::vector<int> im;	// Индексы вариантов с одинаковым объёмом пересечения
	std::vector<int> v;		// Соответсвующие объёмы этим индексам
	for (int i = 0; i < variants.size(); i++)
	{
		if (volumes[i] == volumes[min])
		{
			im.push_back(i);
		}
	}

	// Ищем сумму объёмов для каждого варианта
	for (int i = 0; i < im.size(); i++)
	{
		// Рассчитываем сумму объёмов
		v.push_back(variants[im[i]].first->MBP_.volume() + variants[im[i]].second->MBP_.volume());
#ifdef DEBUG
		TDRtree::operations++;
#endif // DEBUG
	}

	// После чего находим минимальный объём
	min = 0;
	for (int i = 1; i < im.size(); i++)
	{
		if (v[min] > v[i]) min = i;
	}

	// Очищаем все прочие узлы от всех связей и удаляем их
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

	// Добавляем узлы к родителю в список и удаляем старый узел из него
	replace(node, variants[im[min]].first, variants[im[min]].second);
}

void TDRtree::merge(Node* node)
{
	Node* parent = node->parent_;

	std::vector<std::pair<Node*, Node*>> variants;
	variants.push_back(parent->mergeByAxis(node, 0));
	variants.push_back(parent->mergeByAxis(node, 1));
	variants.push_back(parent->mergeByAxis(node, 2));

	// Теперь у нас есть три варианта слияния.
	// Надо выбрать один.
	// В first лежит узел, который поглотит node.
	// В second лежит узел, как будто "уже поглотивший" node
	// Сравниваем second со всеми children кроме node и first

	// Начнём сравнение с объёма пересечения
	std::vector<int> intersections = std::vector<int>(variants.size());

	// Сначала найдём суммарный объём пересечения
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

	// Затем сравним их
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

	// Если у нас только один наименьший вариант, то выбираем именно его
	if (last != min && intersections[min] < intersections[last])
	{
		for (int i = 1; i < variants.size(); i++)
		{
			if (i != min)
			{
				// Очищаем память от уже ненужного мусора
				variants[i].second->breakBounds();
				delete variants[i].second;
			}
		}

		// Перекидываем все узлы из удаляемого узла в другой узел
		for (int i = 0; i < node->children_.size(); i++)
		{
			variants[min].first->add(node->children_[i]);
		}
		// Меняем кубоид на новый
		variants[min].first->MBP_ = variants[min].second->MBP_;
		// Удаляем уже ненужный узел
		variants[min].second->breakBounds();
		delete variants[min].second;
		// Удаляем старый узел
		parent->del(node);
	}

	// Если наименьших вариантов несколько, то их надо выделить
	std::vector<int> mins;

	for (int i = 0; i < intersections.size(); i++)
	{
		if (intersections[i] == intersections[min])
		{
			mins.push_back(i);
		}
	}

	// Из них выделим те, которые дают наименьший прирост в объёме
	std::vector<int> sv = std::vector<int>(mins.size());

	// Посчитаем эту разницу
	for (int i = 0; i < mins.size(); i++)
	{
		sv[i] = variants[mins[i]].second->MBP_.volume() - variants[mins[i]].first->MBP_.volume();
#ifdef DEBUG
		TDRtree::operations++;
#endif // DEBUG
	}

	// Сравним её
	min = mins[0];
	for (int i = 0; i < mins.size(); i++)
	{
		if (sv[mins[i]] < sv[min]) min = sv[mins[i]];
	}

	// Найденный вариант утвердаем
	for (int i = 1; i < variants.size(); i++)
	{
		if (i != min)
		{
			// Очищаем память от уже ненужного мусора
			variants[i].second->breakBounds();
			delete variants[i].second;
		}
	}

	// Перекидываем все узлы из удаляемого узла в другой узел
	for (int i = 0; i < node->children_.size(); i++)
	{
		variants[min].first->add(node->children_[i]);
	}
	// Меняем кубоид на новый
	variants[min].first->MBP_ = variants[min].second->MBP_;
	// Удаляем уже ненужный узел
	variants[min].second->breakBounds();
	delete variants[min].second;
	// Удаляем старый узел
	parent->del(node);
}

void TDRtree::normalize(Node* node)
{
	// Уменьшаем узел (если возможно)
	node->reduce();
	
	// Если количество элементов в узле зашкаливает
	if (node->children_.size() > CHILDREN_MAX_COUNT)
	{
		// то делим узел на две части
		Node* parent = node->parent_;
		split(node);
		if (parent) normalize(parent);
		return;
	}

	// Если количество элементов в узле слишком мало
	if (node->children_.size() < CHILDREN_MIN_COUNT)
	{
		// И узел не является корнем
		if (node == root_) return;

		// То сращиваем его с другим узлом
		Node* parent = node->parent_;
		merge(node);
		if (parent) normalize(parent);
		return;
	}
}

Node* TDRtree::choose(Node* newObject)
{
	// Если корень пуст, то возвращаем его
	//if (root_->isLeaf()) return root_;
	// Иначе ищем среди его потомков
	return chooseNode(root_, newObject);
}

Node* TDRtree::chooseNode(Node* node, Node* newObject)
{
#ifdef DEBUG
	TDRtree::operations++;
#endif // DEBUG
	// Если в искомом узле есть только листья, или если узел является либо пустым корнем, либо корнем с листьями, то возвращаем этот узел
	if (node->childrenAreLeafs() || (node == root_ && root_->isLeaf()))
	{
		// Предварительно, узел следует увеличить, если нужно
		node->MBP_ = node->MBP_.extend(newObject->MBP_);
		return node;
	}

	// Выбираем наиболее подходящий дочерний узел и возвращаем его
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
	// Если данный узел содержит точку, то добавляем его в список, рассматриваем его потомков
	if (node->MBP_.contains(point))
	{
		s->push(node);
		// Если узел является конечным, то сматываем удочки
		if (node->isLeaf()) return;

		// Если мы нашли дочерний узел, содержащий эту точку, то рассматриваем и его так же, как и этот узел
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
	// Если данный узел пересекает область, то добавляем его в список, рассматриваем его потомков
	if (node->MBP_.overlapping(parallelepiped))
	{
		s->push(node);
		// Если узел является конечным, то сматываем удочки
		if (node->isLeaf()) return;

		// Если мы нашли дочерний узел, пересекающий заданную область, то рассматриваем и его так же, как и этот узел
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
	// Если мы уже нашли столько, сколько хотели, то выходим из метода
	if (*max <= *count) return;

	// Если узел является листом, то добавляем его в массив, увеличиваем счётчик, и выходим из метода
	if (node->isLeaf())
	{
		nodes->push_back(node);
		(*count)++;
		return;
	}

	// Если у узла есть потомки, тогда надо рассмотреть их всех, начав с самого ближнего
	// Для этого отсортируем потомков в порядке увеличения дистанции до них

	std::vector<int> distances = std::vector<int>(node->children_.size());
	std::vector<int> indexes = std::vector<int>(node->children_.size());

	// Готовим два массива для дистанций и для осортированных индексов
	for (int i = 0; i < node->children_.size(); i++)
	{
		distances[i] = node->children_[i]->distance(point);
#ifdef DEBUG
		TDRtree::operations++;
#endif // DEBUG
		indexes[i] = i;
	}

	// Сортировка
	for (int i = 0; i < indexes.size() - 1; i++)
	{
		for (int j = i + 1; j < indexes.size(); j++)
		{
			// Если дистанция следующего элемента меньше текущего минимального, то меняем их местами
			if (distances[indexes[j]] < distances[indexes[i]])
			{
				int tmp = indexes[j];
				indexes[j] = indexes[i];
				indexes[i] = tmp;
			}
		}
	}

	// Теперь пробегаемся по отсортированным элементам:
	for (int i = 0; i < indexes.size(); i++)
	{
		// Рассматриваем очередной элемент
		cluster(point, node->children_[indexes[i]], nodes, max, count);
#ifdef DEBUG
		TDRtree::operations++;
#endif // DEBUG

		// Если мы уже нашли столько, сколько хотели, то выходим из метода
		if (*max <= *count) return;
	}

	// Если все дочерние узлы были рассмотренны, и ещё нужны соседи, то добавляем себя в массив соседей
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