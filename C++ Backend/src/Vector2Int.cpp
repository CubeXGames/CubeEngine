#include "Main.h"
#include <math.h>

struct Vector2Int {

public:

	int x, y;

	Vector2Int(int x, int y) {

		this->x = x;
		this->y = y;
	}

	int sqrMagnitude() {

		return x * x + y * y;
	}

	double magnitude() {

		return sqrt(sqrMagnitude());
	}

	Vector2Int& operator+(const Vector2Int& b) {

		Vector2Int v2i = Vector2Int(this->x + b.x, this->y + b.y);
		return v2i;
	}

	Vector2Int& operator-(const Vector2Int& b) {

		Vector2Int v2i = Vector2Int(this->x - b.x, this->y - b.y);
		return v2i;
	}

	Vector2Int& operator*(const Vector2Int& b) {

		Vector2Int v2i = Vector2Int(this->x * b.x, this->y * b.y);
		return v2i;
	}

	Vector2Int& operator/(const Vector2Int& b) {

		Vector2Int v2i = Vector2Int((int)roundf(this->x / (float)b.x), (int)roundf(this->x / (float)b.y));
		return v2i;
	}

	Vector2Int& operator*(const int b) {

		Vector2Int v2i = Vector2Int(this->x * b, this->y * b);
		return v2i;
	}

	Vector2Int& operator/(const float b) {

		Vector2Int v2i = Vector2Int((int)roundf(this->x / b), (int)roundf(this->y / b));
		return v2i;
	}
};