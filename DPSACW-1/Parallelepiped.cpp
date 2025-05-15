#include "Parallelepiped.h"

Parallelepiped::Parallelepiped()
{
	point_ = std::vector<Point>(2);
	point_[0] = *new Point();
	point_[1] = *new Point();
}

Parallelepiped::Parallelepiped(int Ox, int Oy, int Oz)
{
	point_ = std::vector<Point>(2);
	point_[0] = *new Point();
	point_[1] = *new Point(point_[0].getX() + Ox, point_[0].getY() + Oy, point_[0].getZ() + Oz);
}

Parallelepiped::Parallelepiped(Point location, int Ox, int Oy, int Oz)
{
	point_ = std::vector<Point>(2);
	point_[0] = location;
	point_[1] = *new Point(point_[0].getX() + Ox, point_[0].getY() + Oy, point_[0].getZ() + Oz);
}

Parallelepiped::Parallelepiped(Point location, Point end)
{
	point_ = std::vector<Point>(2);
	point_[0] = location;
	point_[1] = end;
}

Parallelepiped::Parallelepiped(Parallelepiped const& parallelepiped)
{
	point_ = parallelepiped.point_;
}

Parallelepiped::~Parallelepiped() {}

Point Parallelepiped::getLocation() const
{
	return point_[0];
}

Point Parallelepiped::location() const
{
	return Point(getX() + getOx() / 2, getY() + getOy() / 2, getZ() + getOz() / 2);
}

Point Parallelepiped::getEnd() const
{
	return point_[1];
}

void Parallelepiped::setEnd(Point point)
{
	point_[1] = point;
}

int Parallelepiped::get(int i, int axis) const
{
	if (i > 1) return point_[1].get(axis);
	return point_[i].get(axis);
}

void Parallelepiped::set(int coordinate, int i, int axis)
{
	if (i > 1) return;
	point_[i].set(coordinate, axis);
}

int Parallelepiped::getX() const
{
	return point_[0].getX();
}

int Parallelepiped::getY() const
{
	return point_[0].getY();
}

int Parallelepiped::getZ() const
{
	return point_[0].getZ();
}

void Parallelepiped::setX(int x)
{
	point_[0].setX(x);
}

void Parallelepiped::setY(int y)
{
	point_[0].setY(y);
}

void Parallelepiped::setZ(int z)
{
	point_[0].setZ(z);
}

int Parallelepiped::getX2() const
{
	return point_[1].getX();
}

int Parallelepiped::getY2() const
{
	return point_[1].getY();
}

int Parallelepiped::getZ2() const
{
	return point_[1].getZ();
}

void Parallelepiped::setX2(int x)
{
	point_[1].setX(x);
}

void Parallelepiped::setY2(int y)
{
	point_[1].setY(y);
}

void Parallelepiped::setZ2(int z)
{
	point_[1].setZ(z);
}

void Parallelepiped::setLocation(Point point)
{
	point_[1] = point;
}

int Parallelepiped::getOx() const
{
	return point_[1].getX() - point_[0].getX();
}

int Parallelepiped::getOy() const
{
	return point_[1].getY() - point_[0].getY();
}

int Parallelepiped::getOz() const
{
	return point_[1].getZ() - point_[0].getZ();
}

void Parallelepiped::setOx(int Ox)
{
	point_[1].setX(point_[0].getX() + Ox);
}

void Parallelepiped::setOy(int Oy)
{
	point_[1].setY(point_[0].getY() + Oy);
}

void Parallelepiped::setOz(int Oz)
{
	point_[1].setZ(point_[0].getZ() + Oz);
}

std::ostream& operator << (std::ostream& out, Parallelepiped const& parallelepiped)
{
	out << parallelepiped.point_[0] << '\n' << parallelepiped.point_[1];
	return out;
}

std::istream& operator >> (std::istream& in, Parallelepiped& parallelepiped)
{
	in >> parallelepiped.point_[0];
	in >> parallelepiped.point_[1];
	return in;
}

int Parallelepiped::volume() const
{
	return (point_[1].getX() - point_[0].getX()) * (point_[1].getY() - point_[0].getY()) * (point_[1].getZ() - point_[0].getZ());
}

int Parallelepiped::area() const
{
	return (point_[1].getX() - point_[0].getX()) * (point_[1].getY() - point_[0].getY()) * 2 + (point_[1].getX() - point_[0].getX()) * (point_[1].getZ() - point_[0].getZ()) * 2 + (point_[1].getY() - point_[0].getY()) * (point_[1].getZ() - point_[0].getZ()) * 2;
}

Parallelepiped Parallelepiped::operator = (Parallelepiped const& parallelepiped)
{
	point_ = parallelepiped.point_;
	return *this;
}

bool Parallelepiped::operator == (Parallelepiped const& parallelepiped) const
{
	return point_ == parallelepiped.point_;
}

bool Parallelepiped::operator != (Parallelepiped const& parallelepiped) const
{
	return volume() != parallelepiped.volume();
}

bool Parallelepiped::operator < (Parallelepiped const& parallelepiped) const
{
	return volume() < parallelepiped.volume();
}

bool Parallelepiped::operator <= (Parallelepiped const& parallelepiped) const
{
	return volume() <= parallelepiped.volume();
}

bool Parallelepiped::operator > (Parallelepiped const& parallelepiped) const
{
	return volume() > parallelepiped.volume();
}

bool Parallelepiped::operator >= (Parallelepiped const& parallelepiped) const
{
	return volume() >= parallelepiped.volume();
}

bool Parallelepiped::contains(Parallelepiped const& parallelepiped) const
{
	return *this == parallelepiped || point_[0] <= parallelepiped.point_[0] && point_[1] >= parallelepiped.point_[1];
}

bool Parallelepiped::contains(Point const& point) const
{
	return point_[0] <= point && point_[1] >= point;
}

bool Parallelepiped::overlapping(Parallelepiped const& parallelepiped) const
{
	return intersection(parallelepiped).volume() > 0;
}

Parallelepiped Parallelepiped::extend(Parallelepiped const& parallelepiped) const
{
	Parallelepiped p(*this);

	if (p.getX() > parallelepiped.getX()) p.setX(parallelepiped.getX());
	if (p.getY() > parallelepiped.getY()) p.setY(parallelepiped.getY());
	if (p.getZ() > parallelepiped.getZ()) p.setZ(parallelepiped.getZ());

	if (p.getX2() < parallelepiped.getX2()) p.setX2(parallelepiped.getX2());
	if (p.getY2() < parallelepiped.getY2()) p.setY2(parallelepiped.getY2());
	if (p.getZ2() < parallelepiped.getZ2()) p.setZ2(parallelepiped.getZ2());

	return p;
}

Parallelepiped Parallelepiped::intersection(Parallelepiped const& parallelepiped) const
{
	Parallelepiped p;

	if (*this == parallelepiped) return *this;

	if (getX() > parallelepiped.getX()) p.setX(getX());
	else p.setX(parallelepiped.getX());

	if (getY() > parallelepiped.getY()) p.setY(getY());
	else p.setY(parallelepiped.getY());

	if (getZ() > parallelepiped.getZ()) p.setZ(getZ());
	else p.setZ(parallelepiped.getZ());

	if (getX2() < parallelepiped.getX2()) p.setX2(getX2());
	else p.setX2(parallelepiped.getX2());

	if (getY2() < parallelepiped.getY2()) p.setY2(getY2());
	else p.setY2(parallelepiped.getY2());

	if (getZ2() < parallelepiped.getZ2()) p.setZ2(getZ2());
	else p.setZ2(parallelepiped.getZ2());

	// Если кубоид правильный, и если он лежит в обоих кубоидах, которые его формируют, то его и возвращаем
	if ((p.getLocation() < p.getEnd()) && contains(p) && parallelepiped.contains(p)) return p;

	return Parallelepiped();
}