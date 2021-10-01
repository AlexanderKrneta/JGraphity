using namespace std;
#include <vector>
#include <iostream>
#include <cmath>
#include <fstream>
#include <bits/stdc++.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "vec.h"
#include "body.h"
#include "particle.h"
#include "quadtree.h"

void step(vector<body> &bodies, float dt);
void addFrame(ofstream &ofs, vector<body> bodies, float percDone);
void HSVtoRGB(char string[], float H, float V);

int main(int argc, char **argv){
    if(argc != 5)
    {
        cout << "Usage: " << argv[0] <<" [body file] [time step size] [steps per frame] [number of steps]\n";
        return -1;
    }
    vector<body> bodies = getBodiesFromFile(argv[1]);
    float dt = atof(argv[2]); // c string to float
    int sPF = atoi(argv[3]), numSteps = atoi(argv[4]); // c string to int
    int numFrames = numSteps/sPF;
    //Create the .jgr name of the outfile
    char jgrName[100]={'\0'};
    unsigned nameL = strlen(argv[1])-3;
    strncpy(jgrName, argv[1], nameL);
    strcat(jgrName, "jgr");
    ofstream ofs(jgrName, std::ofstream::out); // Create a output file stream for the jgr data
    
    cout<< "Time step: " << dt << ". Steps per frame: " << sPF << ". Number of Steps: "<<numFrames<<".\n";
    // Note, the plus one is for the intial frame
    ofs << "newgraph\n\n"; // Have the file start with 'newgraph'

    // We need to find the bounds of the graph, at a ratio of 8.5 to 11
    // Find the x and y bounds, of all of the verticies on every polygon
    double xmin, xmax, ymin, ymax;
    xmin = xmax = bodies[0].polygon[0].x;
    ymin = ymax = bodies[0].polygon[0].y;
    for(unsigned i=0; i<bodies.size(); i++)
    {
        for(unsigned j=0; j<bodies[i].polygon.size(); j++) // bodies[i].polygon[j]
        {
            if(bodies[i].polygon[j].x < xmin) xmin = bodies[i].polygon[j].x;
            else if(bodies[i].polygon[j].x > xmax) xmax = bodies[i].polygon[j].x;
            if(bodies[i].polygon[j].y < ymin) ymin = bodies[i].polygon[j].y;
            else if(bodies[i].polygon[j].y > ymax) ymax = bodies[i].polygon[j].y;
        }
    }
    double xCenter = (xmin+xmax)/2, yCenter = (ymin+ymax)/2;
    if(((ymax-ymin)/(xmax-xmin) > (11.095/7.89)))
    {
        double dx = ((ymax-ymin)/11.095*7.89)/2;
        xmin = xCenter - dx;
        xmax = xCenter + dx;
    }
    else
    {
        double dy = ((xmax-xmin)/7.89*11.095)/2;
        ymin = yCenter - dy;
        ymax = yCenter + dy;
    }
    cout<< xmin <<' '<<xmax<<' '<<ymin<<' '<<ymax<<'\n';
    // Now let us define the x axis and the y axis
    //ofs << "xaxis min "<<xmin<<" max "<<xmax<<" no_draw_hash_marks size 8.5\n";
    //ofs << "yaxis min "<<ymin<<" max "<<ymax<<" no_draw_hash_marks size 11\n";
    ofs << "xaxis min "<<xmin<<" max "<<xmax<<" hash 0 mhash 0 size 7.89\n";
    ofs << "yaxis min "<<ymin<<" max "<<ymax<<" hash 0 mhash 0 size 11.095\n\n";

    //Now let us define the background;
    ofs << "newline poly pcfill 0 0 0 marktype none pts\n"<< 2*xmax <<' '<< 2*ymax <<' '<< 2*xmin <<' '<< 2*ymax <<' '<< 2*xmin <<' '<< 2*ymin <<' '<< 2*xmax <<' '<< 2*ymin << "\n\n";
    
    /*
    char sysCall[500]={'\0'};
    strcat(sysCall, "./j -P ");
    strcat(sysCall, jgrName);
    strcat(sysCall, " | ps2pdf - | convert -density 300 - -quality 100 ");
    char jpgName[100]={'\0'};
    strncpy(jpgName, jgrName, nameL);
    strcat(jpgName, "jpg\n");
    strcat(sysCall, jpgName);
    cout<<sysCall;
    system(sysCall);
    cout<<system("make run");
    */


    addFrame(ofs, bodies, .5);
    for(int i=0; i<numFrames; i++)
    {
        for(int s=0; s<sPF; s++) step(bodies, dt); // step sPF number of times
        addFrame(ofs, bodies, (float)(i)/(float)(numFrames));
    }
    return 0;
}






void step(vector<body> &bodies, float dt)
{
    // create a list to store the forces on each particle, per body
    double SHIFT = 0.3;
    vector<vector<vec>> forces;
    for(unsigned i=0; i<bodies.size(); i++)
    {
        forces.push_back(vector<vec>(bodies[i].masses.size())); // Push back a vector, with (number of masses) number of elements
    }

    for(unsigned i=0; i < bodies.size(); i++) // For every Body
    {
        for(unsigned j=(0); j < bodies[i].masses.size(); j++) // For every particle in the body[i]
        {
            for(unsigned k=0; k < bodies.size(); k++) // For every other body, ahead in the list
            {
                if(i==k) continue;
                for(unsigned l=0; l < bodies[k].masses.size(); l++) // For every particle in this secondary body
                {
                    vec r = bodies[k].masses[l].pos - bodies[i].masses[j].pos; // Get the r, vector
                    double rMag = mag(r);
                    vec F = (((bodies[i].masses[j].mass * bodies[k].masses[l].mass)/(pow((rMag + SHIFT),2)))/rMag) * r; // get the force vector from B_i.j to B_k.l
                    forces[i][j] = forces[i][j] + F; // Forces between the two particles
                    //forces[k][l] = forces[k][l] - F; // Update them
                }
            }
        }
    }

    // Now we should have all of the forces on each particle
    // Now for every body, iterate through the forces, and each particle
    // Every body has a trans force, as well as a rot force
    vector<double> rotF(bodies.size());
    vector<vec> transF(bodies.size());
    for(unsigned i=0; i<bodies.size(); i++) // For every body
    {
        for(unsigned j=0; j<bodies[i].masses.size(); j++) // For every particle inside of that body
        {   
            vec r = bodies[i].COM.pos - bodies[i].masses[j].pos; // Get the vector from partical to the COM
            double rMag = mag(r); // The dist from partical to COM
            vec rHat = r/rMag; // The dir vector from partical to COM
            vec tangential(rHat.y, -1*rHat.x); // Now we have the components for this partical to the COM
            rotF[i] = rotF[i] + (tangential * forces[i][j])*rMag; // Add the rotational force F*r
            transF[i] = transF[i] + forces[i][j]; // Add the force F
        }
    }

    // Now every body has a corresponding translational and rotational force
    for(unsigned i=0; i<bodies.size(); i++) // Update the positions of each body
    {
        // Update velocities, due to accelerations
        bodies[i].rotVel = bodies[i].rotVel + rotF[i]/bodies[i].moI*dt; // Update rotational/angular velocity
        bodies[i].transVel = bodies[i].transVel + transF[i]/bodies[i].mass*dt; // Update the translational velocity
        //cout<<bodies[i].rotVel <<' '<<bodies[i].transVel.str();
        bodies[i].rotate(bodies[i].rotVel*(180/M_PI)*dt); // rotate (turn rads to degrees), times dt
        bodies[i].translate(dt*(bodies[i].transVel)); // Move *dt
    }
}

void addFrame(ofstream &ofs, vector<body> bodies, float percDone)
{
    for(unsigned i=0; i<bodies.size(); i++)
    {
        ofs << "newline poly pcfill ";
        char color[40]={'\0'};
        HSVtoRGB(color, i*360/bodies.size(), percDone);
        ofs << color;
        ofs <<  " marktype none pts\n";
        for(unsigned j=0; j<bodies[i].polygon.size(); j++)
        {
            ofs << bodies[i].polygon[j].x << ' ' << bodies[i].polygon[j].y << ' ';
        }
        ofs << '\n';
    }
}

void HSVtoRGB(char string[], float H, float V){
    if(H>360 || H<0 ||  V>100 || V<0){
        cout<<"The givem HSV values are not in valid range"<<endl;
        return;
    }
    float s = .75, v = V/100, r, g, b; // The s is saturation
    float C = s*v;
    float X = C*(1-abs(fmod(H/60.0, 2)-1));
    float m = v-C;
    if(H >= 0 && H < 60) r = C,g = X,b = 0;
    else if(H >= 60 && H < 120) r = X,g = C,b = 0;
    else if(H >= 120 && H < 180) r = 0,g = C,b = X;
    else if(H >= 180 && H < 240) r = 0,g = X,b = C;
    else if(H >= 240 && H < 300) r = X,g = 0,b = C;
    else r = C,g = 0,b = X;
    float R = (r+m)*100, G = (g+m)*100, B = (b+m)*100;
    char buff[40];
    sprintf(buff, "%f ", R);
    strcat(string, buff);
    sprintf(buff, "%f ", G);
    strcat(string, buff);
    sprintf(buff, "%f ", B);
    strcat(string, buff);
}