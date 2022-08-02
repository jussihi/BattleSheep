#pragma once

class Point2f
{
public:
  Point2f() {}
  Point2f(double x_, double y_) : x(x_), y(y_) {}
  // Point2f& operator=(const Point2f& other) { x = other.x; y = other.y; };
  double x, y;
};

class Point2i
{
public:
  Point2i() {}
  Point2i(int x_, int y_) : x(x_), y(y_) {}
  // Point2i& operator=(const Point2i& other) { x = other.x; y = other.y; };
  int x, y;
};