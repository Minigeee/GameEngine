#include <Math/Matrix2.h>

#include <vld.h>

int main()
{
	Matrix2f a(2.0f);
	Vector2f v(1.0f, 3.0f);

	Vector2f result = a * v;

	return 0;
}