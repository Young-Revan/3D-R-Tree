#pragma once

#include "Point.h"
#include <vector>

class Parallelepiped
{
public:
	Parallelepiped();
	Parallelepiped(int Ox, int Oy, int Oz);
	Parallelepiped(Point location, int Ox, int Oy, int Oz);
	Parallelepiped(Point location, Point end);
	Parallelepiped(Parallelepiped const& parallelepiped);
	~Parallelepiped();

	// ¬озвращает "рабочие" координаты узла
	Point getLocation() const;
	void setLocation(Point point);

	// ¬озвращает приблизительный центр узла
	Point location() const;

	Point getEnd() const;
	void setEnd(Point point);

	int get(int i, int axis) const;
	void set(int coordinate, int i, int axis);

	int getX() const;
	int getY() const;
	int getZ() const;

	void setX(int x);
	void setY(int y);
	void setZ(int z);

	int getX2() const;
	int getY2() const;
	int getZ2() const;

	void setX2(int x);
	void setY2(int y);
	void setZ2(int z);

	int getOx() const;
	int getOy() const;
	int getOz() const;

	void setOx(int Ox);
	void setOy(int Oy);
	void setOz(int Oz);

	friend std::ostream& operator << (std::ostream& out, Parallelepiped const& parallelepiped);
	friend std::istream& operator >> (std::istream& in, Parallelepiped& parallelepiped);

	Parallelepiped operator = (Parallelepiped const& parallelepiped);

	bool operator == (Parallelepiped const& parallelepiped) const;

	bool operator != (Parallelepiped const& parallelepiped) const;
	bool operator < (Parallelepiped const& parallelepiped) const;
	bool operator <= (Parallelepiped const& parallelepiped) const;
	bool operator > (Parallelepiped const& parallelepiped) const;
	bool operator >= (Parallelepiped const& parallelepiped) const;

	int volume() const;
	int area() const;

	// ¬озвращает true, если паралеллипипед содержит точку
	bool contains(Point const& point) const;

	// ¬озвращает true, если паралеллипипед полность содержит в себе другой паралеллипипед
	bool contains(Parallelepiped const& parallelepiped) const;

	// ¬озвращает true, если паралеллипипеды пересекаютс€ друг другом
	bool overlapping(Parallelepiped const& parallelepiped) const;

	// ¬озвращает паралелипипед, €вл€ющийс€ расширенным паралеллипипедом, чтобы вместить в себ€ аргумент
	Parallelepiped extend(Parallelepiped const& parallelepiped) const;

	// ¬озвращает паралелипипед, €вл€ющийс€ пересечением this и аргумента
	Parallelepiped intersection(Parallelepiped const& parallelepiped) const;

private:

private:
	std::vector<Point> point_;
};