#include <iostream>
#include "TDRtree.h"

void add(TDRtree& tree);
void findInRange(TDRtree& tree);
void findClosestNeighbour(TDRtree& tree);
void deleteNode(TDRtree& tree);
bool dosmth(TDRtree& tree);

void firstSet(TDRtree& tree);


int main()
{
	setlocale(LC_ALL, "Russian");

#ifdef DEBUG
	TDRtree::operations = 0;
#endif // DEBUG

	TDRtree tree;

	firstSet(tree);

	while (dosmth(tree));

	system("pause");
	return 0;
}

void add(TDRtree& tree)
{
#ifdef DEBUG
	TDRtree::operations = 0;
#endif // DEBUG
	std::cout << '\n';
	std::string name;
	Point location;
	int ox, oy, oz;
	
	std::cout << "Название: ";
	std::cin >> name;
	std::cout << "Координаты:\n";
	std::cin >> location;
	std::cout << "Размеры:\n";
	std::cin >> ox;
	std::cin >> oy;
	std::cin >> oz;

	tree.add(name, nullptr, Parallelepiped(location, ox, oy, oz));
	std::cout << '\n';
#ifdef DEBUG
	std::cout << "Количество обращений к узлам дерева: " << TDRtree::operations << "\n\n\n";
#endif // DEBUG
}

void findInRange(TDRtree& tree)
{
#ifdef DEBUG
	TDRtree::operations = 0;
#endif // DEBUG
	std::cout << '\n';
	Point location;
	int ox, oy, oz;

	std::cout << "Координаты:\n";
	std::cin >> location;
	std::cout << "Размеры:\n";
	std::cin >> ox;
	std::cin >> oy;
	std::cin >> oz;

	std::vector<Node*> nodes = tree.rangeSearch(Parallelepiped(location, ox, oy, oz));
	for (int i = 0; i < nodes.size(); i++)
	{
		std::cout << nodes[i];
	}
	std::cout << '\n';
#ifdef DEBUG
	std::cout << "Количество обращений к узлам дерева: " << TDRtree::operations << "\n\n\n";
#endif // DEBUG
}

void findClosestNeighbour(TDRtree& tree)
{
#ifdef DEBUG
	TDRtree::operations = 0;
#endif // DEBUG
	std::cout << '\n';
	Point location;
	std::cout << "Текущие координаты:\n";
	std::cin >> location;
	int count;
	std::cout << "Количество желаемых результатов: ";
	std::cin >> count;

	std::vector<Node*> nodes = tree.closestNeighbours(location, count);
	for (int i = 0; i < nodes.size(); i++)
	{
		std::cout << nodes[i];
	}
	std::cout << '\n';
#ifdef DEBUG
	std::cout << "Количество обращений к узлам дерева: " << TDRtree::operations << "\n\n\n";
#endif // DEBUG
}

void deleteNode(TDRtree& tree)
{
#ifdef DEBUG
	TDRtree::operations = 0;
#endif // DEBUG
	std::cout << '\n';
	std::string name;
	Point location;
	int ox, oy, oz;

	std::cout << "Название: ";
	std::cin >> name;
	std::cout << "Координаты:\n";
	std::cin >> location;
	std::cout << "Размеры:\n";
	std::cin >> ox;
	std::cin >> oy;
	std::cin >> oz;

	tree.del(name, Parallelepiped(location, ox, oy, oz));
	std::cout << '\n';
#ifdef DEBUG
	std::cout << "Количество обращений к узлам дерева: " << TDRtree::operations << "\n\n\n";
#endif // DEBUG
}

bool dosmth(TDRtree& tree)
{
	std::cout << "1. Показать дерево\n2. Добавить объект\n3. Найти объекты в заданной области\n4. Найти ближайших соседей\n5. Удалить объект\n0. Выход\n";
	
	int command;
	std::cin >> command;

	switch (command)
	{
	case 1:
		std::cout << tree << "\n\n";
		break;

	case 2:
		add(tree);
		break;

	case 3:
		findInRange(tree);
		break;

	case 4:
		findClosestNeighbour(tree);
		break;

	case 5:
		deleteNode(tree);
		break;

	case 0:
		return false;
		break;
	}

	return true;
}

void firstSet(TDRtree& tree)
{
	tree.add("A", nullptr, Parallelepiped(Point(12, 25, 10), 10, 10, 5));
	tree.add("B", nullptr, Parallelepiped(Point(-5, 5, 5), 3, 3, 3));
	tree.add("C", nullptr, Parallelepiped(Point(-40, -30, -50), 15, 10, 8));
	tree.add("D", nullptr, Parallelepiped(Point(50, 60, 50), 20, 30, 15));
	tree.add("E", nullptr, Parallelepiped(Point(80, -50, -5), 17, 8, 21));
	tree.add("F", nullptr, Parallelepiped(Point(25, 50, 10), 5, 5, 10));
	tree.add("G", nullptr, Parallelepiped(Point(0, -40, 0), 20, 15, 25));
	tree.add("H", nullptr, Parallelepiped(Point(50, 8, 0), 5, 5, 10));
	tree.add("I", nullptr, Parallelepiped(Point(20, 5, -6), 11, 7, 12));
	tree.add("J", nullptr, Parallelepiped(Point(30, 30, 40), 10, 15, 10));

	

	std::cout << tree << "\n\n";
}