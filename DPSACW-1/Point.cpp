#include "Point.h"

Point::Point()
{
	coordinates_ = std::vector<int>(3);
}

Point::Point(int x, int y, int z)
{
	coordinates_ = std::vector<int>(3);
	coordinates_[0] = x;
	coordinates_[1] = y;
	coordinates_[2] = z;
}

Point::Point(Point const& point)
{
	coordinates_ = point.coordinates_;
}

Point::~Point() {}

int Point::getX() const
{
	return coordinates_[0];
}

int Point::getY() const
{
	return coordinates_[1];
}

int Point::getZ() const
{
	return coordinates_[2];
}

void Point::setX(int x)
{
	coordinates_[0] = x;
}

void Point::setY(int y)
{
	coordinates_[1] = y;
}

void Point::setZ(int z)
{
	coordinates_[2] = z;
}

int Point::get(int axis) const
{
	if (axis > 2) return coordinates_[2];
	return coordinates_[axis];
}

void Point::set(int coordinate, int axis)
{
	if (axis > 2) return;
	coordinates_[axis] = coordinate;
}

Point Point::operator = (Point const& point)
{
	coordinates_ = point.coordinates_;
	return *this;
}

std::ostream& operator << (std::ostream& out, Point const& point)
{
	out << point.coordinates_[0] << '\n' << point.coordinates_[1] << '\n' << point.coordinates_[2];
	return out;
}

std::istream& operator >> (std::istream& in, Point& point)
{
	in >> point.coordinates_[0];
	in >> point.coordinates_[1];
	in >> point.coordinates_[2];
	return in;
}

bool Point::operator == (Point const& point) const
{
	return coordinates_ == point.coordinates_;
}

bool Point::operator != (Point const& point) const
{
	return !(*this == point);
}

bool Point::operator < (Point const& point) const
{
	return coordinates_[0] < point.coordinates_[0] && coordinates_[1] < point.coordinates_[1] && coordinates_[2] < point.coordinates_[2];
}

bool Point::operator <= (Point const& point) const
{
	return (coordinates_[0] < point.coordinates_[0] || coordinates_[0] == point.coordinates_[0]) && (coordinates_[1] < point.coordinates_[1] || coordinates_[1] == point.coordinates_[1]) && (coordinates_[2] < point.coordinates_[2] || coordinates_[2] == point.coordinates_[2]);
}

bool Point::operator > (Point const& point) const
{
	return coordinates_[0] > point.coordinates_[0] && coordinates_[1] > point.coordinates_[1] && coordinates_[2] > point.coordinates_[2];
}

bool Point::operator >= (Point const& point) const
{
	return (coordinates_[0] > point.coordinates_[0] || coordinates_[0] == point.coordinates_[0]) && (coordinates_[1] > point.coordinates_[1] || coordinates_[1] == point.coordinates_[1]) && (coordinates_[2] > point.coordinates_[2] || coordinates_[2] == point.coordinates_[2]);
}

Point Point::operator + (Point const& point) const
{
	return Point(coordinates_[0] + point.coordinates_[0], coordinates_[1] + point.coordinates_[1], coordinates_[2] + point.coordinates_[2]);
}

Point Point::operator - (Point const& point) const
{
	return Point(coordinates_[0] - point.coordinates_[0], coordinates_[1] - point.coordinates_[1], coordinates_[2] - point.coordinates_[2]);
}