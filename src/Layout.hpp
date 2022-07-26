#pragma once

#include "Point.hpp"

class Layout
{
public:
  Layout(Orientation orientation_, Point size_, Point origin_) :
    orientation(orientation_), size(size_), origin(origin_) {}

  const Orientation orientation;
  const Point size;
  const Point origin;
};