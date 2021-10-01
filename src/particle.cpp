#include "particle.h"
#include "vec.h"
#include <iostream>

particle::particle(){
    pos = vec();
    mass = 0;
}

particle::particle(const vec &r, const double m){
    pos = r;
    mass = m;
}

particle::particle(double m){
    pos = vec();
    mass = m;
}

particle::particle(const vec &r){
    pos = r;
    mass = 1;
}

particle::particle(const particle &p){
    mass = p.mass;
    pos = p.pos;
}

particle operator+(const particle &lhs, const particle &rhs){ return particle((lhs.mass*lhs.pos + rhs.mass*rhs.pos)/(lhs.mass + rhs.mass), lhs.mass + rhs.mass); }

particle operator+(const vec &lhs, const particle &rhs){ return lhs + rhs.pos, rhs.mass; }

particle operator+(const particle &lhs, const vec &rhs){ return rhs + lhs.pos, lhs.mass; }