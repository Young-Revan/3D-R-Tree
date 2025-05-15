#pragma once

#include <iostream>
#include <vector>

class Point
{
public:
	Point();
	Point(int x, int y, int z);
	Point(Point const& point);
	virtual ~Point();

	int getX() const;
	int getY() const;
	int getZ() const;

	void setX(int x);
	void setY(int y);
	void setZ(int z);

	int get(int axis) const;
	void set(int coordinate, int axis);

	friend std::ostream& operator << (std::ostream& out, Point const& point);
	friend std::istream& operator >> (std::istream& in, Point& point);

	Point operator = (Point const& point);

	bool operator == (Point const& point) const;
	bool operator != (Point const& point) const;
	bool operator < (Point const& point) const;
	bool operator <= (Point const& point) const;
	bool operator > (Point const& point) const;
	bool operator >= (Point const& point) const;

	Point operator + (Point const& point) const;
	Point operator - (Point const& point) const;

protected:

protected:
	std::vector<int> coordinates_;
};