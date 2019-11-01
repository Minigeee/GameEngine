#ifndef RECT_H
#define RECT_H

#include <Core/DataTypes.h>

///////////////////////////////////////////////////////////////////////////////

template <typename T>
struct Rect
{
	Rect() = default;
	Rect(T x, T y, T w, T h) :
		x(x),
		y(y),
		w(w),
		h(h)
	{ }

	bool Contains(const Rect<T>& r)
	{
		return
			x < r.x + r.w &&
			x + w > r.x &&
			y < r.y + r.h &&
			y + h > r.y;
	}

	T x, y, w, h;
};

///////////////////////////////////////////////////////////////////////////////

typedef Rect<float>		Rectf;
typedef Rect<double>	Rectd;
typedef Rect<int>		Recti;
typedef Rect<Uint32>	Rectu;

///////////////////////////////////////////////////////////////////////////////

#endif