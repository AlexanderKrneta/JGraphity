#pragma once

using namespace std;
#include <iostream>
#include <string>

class vec{
    public:
        double x, y;
        vec();
        vec(double x, double y);
        vec(const vec &r);
        friend double operator*(const vec &lhs, const vec &rhs); // Dot lhs with rhs
        friend vec operator*(const vec &lhs, const double rhs); // Scale lhs with rhs
        friend vec operator*(const double lhs, const vec &rhs); // Scale rhs with lhs
        friend vec operator+(const vec &lhs, const vec &rhs); // Add 2 vecs together
        friend vec operator-(const vec &lhs, const vec &rhs); // Subtract rhs
        friend vec operator/(const vec &lhs, const double rhs); // Divide by rhs
        friend double cross(const vec &lhs, const vec &rhs); // Cross Product z val
        friend double mag2(const vec &r);
        friend double mag(const vec &r);
        void print(); // Print vector
        string str(); // Turns 
        vec rotate(double t);
};

