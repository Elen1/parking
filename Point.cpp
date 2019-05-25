#include "Point.h"
using namespace app;


Point::Point(int x,int y)
	:x_(x),
	y_(y)
{
}


Point::~Point()
{
}

void Point::set_x(const int x) {
	if (x < 0) {
		return;
	}
	x_ = x;
}
void Point::set_y(const int y) {
	if (y < 0) {
		return;
	}
	y_ = y;
}

int Point::get_x()const {
	return x_;
}
int Point::get_y()const {
	return y_;
}
