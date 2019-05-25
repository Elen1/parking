#ifndef _POINT_H__
#define _POINT_H__

namespace app
{

	class Point
	{
	public:
		Point(int x = 0, int y = 0);
		Point(const Point& other) = default;
		Point(Point&& other) = default;
		~Point();

		void set_x(const int x);
		void set_y(const int y);

		int get_x()const;
		int get_y()const;

	private:
		int x_;
		int y_;
	};

}



#endif // !_POINT_H__


