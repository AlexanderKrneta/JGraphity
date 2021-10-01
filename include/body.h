#pragma once

using namespace std;
#include <vector>
#include <string>
#include "vec.h"
#include "particle.h"
#include <cmath>

class body{
    public:
        double mass, moI, rotVel;
        vec transVel;
        particle COM;
        vector<vec> polygon;
        vector<particle> masses;
        void translate(vec r);
        void rotate(double t);
        void rotate(double t, vec axis);
        body(vector<vec> &verticies);
        string str();
        void calculateCOM();
        void calculateMoI();
};

struct line{
    double error = .001; // If the double is within error of int, turn to int.  error=.001 if x.001 = x   &   x.999 = x+1
    vec r0, v;
    vec interceptHorz(float y){ // Define point error if x.001 = x   &   x.999 = x+1
        double x = ((y - r0.y)/v.y)*v.x + r0.x;
        if(x > ceil(x) - error) x = (int)(x+1); // If just below an int
        else if(x < floor(x) + error) x = (int)x; // if just above an int
        return vec(x, y);
    }
    vec interceptVert(float x){
        double y = ((x - r0.x)/v.x)*v.y + r0.y;
        if(y > ceil(y) - error) y = (int)(y+1); // If just below an int
        else if(y < floor(y) + error) y = (int)y; // if just above an int
        return vec(x, y);
    }
};

struct square{
    line *recentLine;
    particle part;
    int type = 0; // 0-undefined 1-filling 2-perimieter 
    vector<line> lines;
};

vector<body> getBodiesFromFile(string fileName);