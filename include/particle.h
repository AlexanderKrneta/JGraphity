#pragma once

#include "vec.h"

class particle{
    public:
        vec pos;
        double mass;
        particle();
        particle(const vec &r, const double m);
        particle(double m);
        particle(const vec &r);
        particle(const particle &p);
        friend particle operator+(const particle &lhs, const particle &rhs);
        friend particle operator+(const vec &lhs, const particle &rhs);
        friend particle operator+(const particle &lhs, const vec &rhs);
};