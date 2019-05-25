#include "Rectangle.h"
using namespace app;

Rectangle::Rectangle(const Point& lt, int width, int height)
	:left_top_(lt),
	width_(width),
	height_(height),
	empty_(true)
{

}
Rectangle::~Rectangle() {

}


void Rectangle::set_left_top(const Point& other) {
	left_top_.set_x(other.get_x());
	left_top_.set_y(other.get_y());
}

void Rectangle::set_width(int width){
	width_ = width;
}
void Rectangle::set_height(int height){
	height_ = height;
}

bool Rectangle::isEmpty()const {
	return empty_;
}

void Rectangle::set_state(bool e) {
	empty_ = e;
}

const Point& Rectangle::get_left_top() const {
	return left_top_;
}
int Rectangle::get_width()const {
	return width_;
}
int Rectangle::get_height()const {
	return height_;
}