#include "vec.h"
#include <cmath>

vec::vec(){
    x = 0;
    y = 0;
}

vec::vec(const double rx, const double ry){
    x = rx;
    y = ry;
}

vec::vec(const vec &r){
    x = r.x;
    y = r.y;
}

double operator*(const vec &lhs, const vec &rhs){ return lhs.x*rhs.x + lhs.y*rhs.y; }

vec operator*(const vec &lhs, const double rhs){ return vec(lhs.x*rhs, lhs.y*rhs); }

vec operator*(const double lhs, const vec &rhs){ return vec(rhs.x*lhs, rhs.y*lhs); }

vec operator+(const vec &lhs, const vec &rhs){ return vec(lhs.x + rhs.x, lhs.y + rhs.y); }

vec operator-(const vec &lhs, const vec &rhs){ return vec(lhs.x - rhs.x, lhs.y - rhs.y); }

vec operator/(const vec &lhs, const double rhs){ return lhs*(1/rhs); }

void vec::print(){ cout << this->str(); }

string vec::str(){ return ("< " + to_string(x) + " , " + to_string(y) + " >"); }

vec vec::rotate(double t)
{
    double rads = t*M_PI/180;
    return vec(x*cos(rads) - y*sin(rads), x*sin(rads) + y*cos(rads));
}

double cross(const vec &lhs, const vec &rhs){ return lhs.x*rhs.y - lhs.y*rhs.x; }

double mag2(const vec &r){ return r*r; }

double mag(const vec &r){ return sqrt(r*r); }
