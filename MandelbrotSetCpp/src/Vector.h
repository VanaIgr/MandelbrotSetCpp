#pragma once

#include<stdint.h>

struct vec2
{
	float x, y;

public:
	constexpr vec2(const float x_, const float y_) : x(x_), y(y_) {};

	inline constexpr vec2 operator+(const vec2& o) const {
		return vec2(x + o.x, y + o.y);
	}
	inline constexpr vec2 operator-(const vec2& o) const {
		return vec2(x - o.x, y - o.y);
	}
	inline constexpr vec2 operator/(const vec2& o) const {
		return vec2(x / o.x, y / o.y);
	}
	inline constexpr vec2 operator/(const float o) const {
		return vec2(x / o, y / o);
	}
	inline constexpr vec2 operator*(const vec2& o) const {
		return vec2(x * o.x, y * o.y);
	}
	inline constexpr vec2 operator*(const float o) const {
		return vec2(x * o, y * o);
	}
	inline vec2& operator+=(const vec2& o) {
		x += o.x;
		y += o.y;
		return *this;
	}
};

struct dvec2
{
	double x, y;

public:
	constexpr dvec2(const double x_, const double y_) : x(x_), y(y_) {};
	constexpr dvec2(const vec2 o) : x(o.x), y(o.y) {};

	inline constexpr dvec2 operator+(const dvec2& o) const {
		return dvec2(x + o.x, y + o.y);
	}
	inline constexpr dvec2 operator-(const dvec2& o) const {
		return dvec2(x - o.x, y - o.y);
	}
	inline constexpr dvec2 operator/(const dvec2& o) const {
		return dvec2(x / o.x, y / o.y);
	}
	inline constexpr dvec2 operator/(const double o) const {
		return dvec2(x / o, y / o);
	}
	inline constexpr dvec2 operator*(const dvec2& o) const {
		return dvec2(x * o.x, y * o.y);
	}
	inline constexpr dvec2 operator*(const double o) const {
		return dvec2(x * o, y * o);
	}
	inline constexpr dvec2 operator-() const {
		return dvec2(-x, -y);
	}
	inline dvec2& operator+=(const dvec2& o) {
		x += o.x;
		y += o.y;
		return *this;
	}
};


struct vec2i
{
	int32_t x, y;

public:
	inline constexpr vec2i(const int32_t x_, const int32_t y_) : x(x_), y(y_) {};

	inline constexpr vec2i operator+(const vec2i o) const {
		return vec2i(x + o.x, y + o.y);
	}
	inline constexpr vec2i operator-(const vec2i o) const {
		return vec2i(x - o.x, y - o.y);
	}
};

struct vec2ui
{
	uint32_t x, y;

public:
	inline constexpr vec2ui(const uint32_t x_, const uint32_t y_) : x(x_), y(y_) {};

	inline constexpr vec2ui operator+(const vec2ui o) const {
		return vec2ui(x + o.x, y + o.y);
	}
	inline constexpr vec2ui operator-(const vec2ui o) const {
		return vec2ui(x - o.x, y - o.y);
	}
};