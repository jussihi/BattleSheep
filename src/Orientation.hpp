#pragma once

#include <cmath>    /* std::sqrt */

class Orientation
{
public:
  Orientation(double f0_, double f1_, double f2_, double f3_,
              double b0_, double b1_, double b2_, double b3_,
              double start_angle_) :
    f0(f0_), f1(f1_), f2(f2_), f3(f3_),
    b0(b0_), b1(b1_), b2(b2_), b3(b3_),
    start_angle(start_angle_) {}

    const double f0, f1, f2, f3;
    const double b0, b1, b2, b3;
    const double start_angle; // in multiples of 60 degrees
}


const Orientation layout_pointy
  = Orientation(std::sqrt(3.0), std::sqrt(3.0) / 2.0, 0.0, 3.0 / 2.0,
                std::sqrt(3.0) / 3.0, -1.0 / 3.0, 0.0, 2.0 / 3.0,
                0.5);

const Orientation layout_flat
  = Orientation(3.0 / 2.0, 0.0, std::sqrt(3.0) / 2.0, std::sqrt(3.0),
                2.0 / 3.0, 0.0, -1.0 / 3.0, std::sqrt(3.0) / 3.0,
                0.0);
