#ifndef _RECTANGLE_H__
#define _RECTANGLE_H__
#include "Point.h"

namespace app
{

	class Rectangle
	{
	public:
		Rectangle(const Point&, int, int);
		~Rectangle();

	public:

		void set_left_top(const Point&);

		const Point& get_left_top() const;
		int get_width()const;
		int get_height()const;

		void set_width(int width);
		void set_height(int height);
		void set_state(bool);

		bool isEmpty()const;

	private:
		Point left_top_;
		int width_;
		int height_;
		bool empty_;
	};

}


#endif // !_RECTANGLE_H__



