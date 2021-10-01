#include "body.h"
#include <fstream>
#include <iostream>

 
body::body(vector<vec> &verticies){ polygon = verticies; }

string body::str(){
    string temp;
    for(unsigned i=0; i < polygon.size(); i++){
        temp += polygon[i].str();
    }
    return temp;
}

vector<body> getBodiesFromFile(string fileName){
    // First open the file
    cout<<fileName<<'\n';
    ifstream f(fileName);
    if(!f){                                         // If the file couldn't be opened
      cerr << "Error: file could not be opened\n";
      exit(1);
    }

    // The first line should specify what power
    // of 2 long the axis is. This will be used
    // to properly geberate point masses to
    // approximate the polygon.
    string lineName;                                // This will be used to make sure the format of the file is correct
    f >> lineName;
    cout<<fileName << ' ' << lineName <<'\n';
    if(lineName.compare("AxisPower:")){             // If the file does not have correct first line formate
        cerr << "Error: first line should be 'AxisPower: <unsigned int>'\n";
        exit(1);
    }
    unsigned exponent;
    f >> exponent;
    unsigned axisLength = 1<<exponent;              // The axis length will be 2^exponent

    // Now that we have the axisLength, we will
    // ensure that every vector component is
    // not greater than it.
    int numV, x, y;
    vector<body> bodies;
    while(true)                                     // until we break out of loop, when eof is reached
    {
        // Make sure that the first line starts with 'NumVerticies:'
        f >> lineName;
        if(!lineName.compare("Shift:")) break;
        if(lineName.compare("NumVerticies:")){      // If the file does not have correct first line formate
            cerr << "Error: new body line should be 'NumVerticies: <unsigned int>'\n";
            exit(1);
        }
        f >> numV;
        // Make sure that the next line starts with 'Verticies:'
        vector<vec> verticies(numV);
        f >> lineName;
        if(lineName.compare("Verticies:")){      // If the file does not have correct first line formate
            cerr << "Error: 'Verticies:' line should immediately follow the <unsigned int> provided by 'NumVerticies:'\n";
            exit(1);
        }
        for(int i=0; i < numV; i++)        // For every pair of elements, ensure in range, and append it to 'verticies'
        {
            f >> x;
            if(abs(x) > axisLength) break;
            f >> y;
            if(abs(y) > axisLength) break;
            verticies[i] = vec(x,y);
        }
        f >> lineName;
        if(lineName.compare("Rotate:")){      // If next line is not a rotate value
            cerr << "Error: The next line after 'Verticies:' should be 'Rotate:'\n";
            exit(1);
        }
        int rotate;
        f >> rotate;

        f >> lineName;
        if(lineName.compare("Shift:")){      // If next line is not a rotate value
            cerr << "Error: The next line after 'Rotate:' should be 'Shift:'\n";
            exit(1);
        }
        vec shift;
        f >> shift.x >> shift.y;

        body temp(verticies);
        // Now we need to generate all of the particles for the body
        // First, find the perimeter created by connecting
        // lines through each consecutive point
        vector<vector<square>> board( 2*axisLength, vector<square> (2*axisLength) );
        vector<line> listOfLines(numV);
        for(int i=0; i<numV; i++) // For every consecutive pair on the polygon
        {
            cout<< "Starting Line " << i + 1<< " of " << numV << '\n';
            // Create a line that intercepts those two points
            line tempLine;
            tempLine.r0 = verticies[i];
            tempLine.v = verticies[(i+1)%numV] - verticies[i];
            listOfLines[i] = tempLine;
            //cout << "Line starts at: "<< tempLine.r0.str() << "\n   And goes to: " << tempLine.v.str() << '\n';

            // CRAWLER
            // Determine what "Moves" can be made
            int xMove=0, yMove=0;
            if(tempLine.v.x > 0) xMove = 1;
            if(tempLine.v.x < 0) xMove = -1;
            if(tempLine.v.y > 0) yMove = 1;
            if(tempLine.v.y < 0) yMove = -1;

            // Calculate starting and ending square
            vec start(tempLine.r0 + vec(axisLength, axisLength)), end(tempLine.r0 + tempLine.v + vec(axisLength, axisLength));
            if( tempLine.v.x > 0 && tempLine.v.y >= 0 ){
                if(tempLine.v.y == 0) end = end + vec(0, 1);
                end = end + vec(-1, -1);
            }
            else if( tempLine.v.x <= 0 && tempLine.v.y > 0 ){
                start = start + vec(-1, 0);
                if(tempLine.v.x == 0) end = end + vec(-1, 1);
                end = end + vec(0, -1);
            }
            else if( tempLine.v.x < 0 && tempLine.v.y <= 0 ){
                start = start + vec(-1, -1);
                if(tempLine.v.y == 0) end = end + vec(0, -1);
            }
            else if( tempLine.v.x >= 0 && tempLine.v.y < 0 ){
                start = start + vec(0, -1);
                if(tempLine.v.x == 0) end = end + vec(1, 0);
                end = end + vec(-1, 0);
            }

            //cout << "Start: " << start.str() << " End: " << end.str()<< "\n";

            // Now start the PROCESS of crawling throught the boards until we hit the end.
            vec currSqr = start; // Start at the first square // IT IS LATE SO WE WILL NOT IMPLEMENT THE ALGORITHM
            board[currSqr.x][currSqr.y].type = 2; // Let the start square start the perimeter
            while(!(currSqr.x == end.x && currSqr.y == end.y)) // If 
            {
                //cout<<"Current Square is " << currSqr.str() << '\n';
                if(tempLine.v.x == 0) // If the line is vertical
                {
                    currSqr.y += yMove;
                    board[currSqr.x][currSqr.y].type = 2;
                    continue;
                }
                if(tempLine.v.y == 0) // If the line is horizontal
                {
                    currSqr.x += xMove;                     // Move to this nex square
                    board[currSqr.x][currSqr.y].type = 2;   // Define this next square to be a perimeter
                    continue;  
                }
                vec coordEquiv((float)currSqr.x - (float)axisLength + 0.5, (float)currSqr.y - (float)axisLength + 0.5);
                // Test the move options
                // CAN WE MOVE HORIZONTALLY
                vec intercept = tempLine.interceptVert(coordEquiv.x + 0.5*xMove); // Move in the possible direction, and check the vertical line
                if( abs(intercept.y - coordEquiv.y) < 0.5) // If it intercepts the side of the current box
                {
                    currSqr.x += xMove;                     // Move to this nex square
                    board[currSqr.x][currSqr.y].type = 2;   // Define this next square to be a perimeter
                    continue;                               // Start from the top again
                }
                // CAN WE MOVE VERTICALLY
                intercept = tempLine.interceptHorz(coordEquiv.y + 0.5*yMove); 
                if( abs(intercept.x - coordEquiv.x) < 0.5)
                {
                    currSqr.y += yMove;
                    board[currSqr.x][currSqr.y].type = 2;
                    continue;
                }
                else if(yMove == xMove) // Manuver both x and y, but y first
                {
                    currSqr.y += yMove;
                    board[currSqr.x][currSqr.y].type = 2;
                    currSqr.x += xMove;
                    board[currSqr.x][currSqr.y].type = 2;
                    continue;
                }
                else // but x first
                {
                    currSqr.x += xMove;
                    board[currSqr.x][currSqr.y].type = 2;
                    currSqr.y += yMove;
                    board[currSqr.x][currSqr.y].type = 2;
                    continue;
                }
            }
            for(int yy = (int)(2*axisLength) - 1; yy>=0; yy--)
            {
                for(int xx=0; xx < (int)(2*axisLength); xx++)
                {
                    if(board[xx][yy].type == 2) cout<< "- ";
                    else cout <<"  ";
                }
                cout<<'\n';
            }
        }
        cout<<"Fill\n";
        // Now for every point, test if it is within the polygon
        for(int i=0; i < (int)(2*axisLength); i++) // This will be the y value from bottom to top
        {
            for(int j=0; j < (int)(2*axisLength); j++) // This will be the x valye from left to right
            {
                if(board[j][i].type == 2) continue;
                int count = 0;
                vec coordEquiv((float)j - (float)axisLength + 0.5, (float)i - (float)axisLength + 0.5);
                //Find all of the lines the ray from left to right may intercept
                for(unsigned k=0; k < listOfLines.size(); k++)
                {
                    if(listOfLines[k].r0.x > coordEquiv.x || (listOfLines[k].r0.x + listOfLines[k].v.x) > coordEquiv.x) // If either point of the line is to the right of the line
                    {
                        if(listOfLines[k].r0.y > coordEquiv.y)
                        {
                            if((listOfLines[k].r0.y + listOfLines[k].v.y) < coordEquiv.y)
                            {
                                if((listOfLines[k].interceptHorz(coordEquiv.y)).x > coordEquiv.x)
                                {
                                    count++;
                                }
                            }
                        }
                        else
                        {
                            if((listOfLines[k].r0.y + listOfLines[k].v.y) > coordEquiv.y)
                            {
                                if((listOfLines[k].interceptHorz(coordEquiv.y)).x > coordEquiv.x)
                                {
                                    count++;
                                }
                            }
                        }
                    }
                }
                // If k is odd, turn the board spot type to 1
                if(count%2) board[j][i].type = 1;
            }
        }

        // This just prints off the boards after fill
        for(int yy = (int)(2*axisLength) - 1; yy>=0; yy--)
        {
            for(int xx=0; xx < (int)(2*axisLength); xx++)
            {
                if(board[xx][yy].type == 2) cout<< "- ";
                else if(board[xx][yy].type == 1) cout<< "0 ";
                else cout <<"  ";
            }
            cout<<'\n';
        }

        // NOW we should have a 2Ax by 2Ax board, each labeled either none, fill, perimieter
        // Let us (compress) the board
        // First generate a vector of the particle boards, of fill particles (not perimeter) 3 dimensional tensor
        cout<<"Compress\n";
        vector<vector<particle>> pBoard(axisLength*2, vector<particle>(axisLength*2));

        // Add the particles from the boards to pBoards[0]
        for(int i=0; i<(int)(2*axisLength); i++) // For every row
        {
            for(int j=0; j<(int)(2*axisLength); j++) // For ever element in that row
            {
                if(board[j][i].type == 1) pBoard[j][i] = particle(vec((float)j - (float)axisLength + 0.5, (float)i - (float)axisLength + 0.5), 1); // This gets all of the fill particles
/*############*/else if(board[j][i].type == 2) 
                {
                    temp.masses.push_back(particle(vec((float)j - (float)axisLength + 0.5, (float)i - (float)axisLength + 0.5), 1)); // Rid of this line later for future AA alg
                }
            }
        }
        //cout<<"About to compress\n";
        // Now that we have a the initial board filled
        int l = pBoard.size(); // Get the length of this square boards
        for(int i=0 ; i<(int)(exponent+1); i++)    // For every degree of the board, excluding the last board, 1x1
        {
            //cout<<"Step size is " << (1<<(i+1)) << ". While the mass we are checking for is "<< (1<<(2*i))<<". Doing "<< l/(1<<(i+1)) <<" steps.\n";
            //cout<<"Stopping at "<<l/(1<<(i+1))<<'\n';
            for(int yy=0; yy < l; yy+=(1<<(i+1)))
            {
                for(int xx=0; xx < l; xx+=(1<<(i+1)))
                {   
                    //cout<<xx<<' '<<yy<< '\n';
                    //cout<<' '<< pBoard[xx][yy].mass <<' '<< pBoard[xx+(1<<i)][yy].mass <<' '<< pBoard[xx][yy+(1<<i)].mass <<' '<< pBoard[xx+(1<<i)][yy+(1<<i)].mass <<'\n';
                    if(pBoard[xx]       [yy]       .mass==(1<<(2*i))  // If the particle exists with the proper mass for such a power
                    && pBoard[xx+(1<<i)][yy]       .mass==(1<<(2*i)) 
                    && pBoard[xx]       [yy+(1<<i)].mass==(1<<(2*i)) 
                    && pBoard[xx+(1<<i)][yy+(1<<i)].mass==(1<<(2*i)))
                    {
                        //cout<<"Compression Occured!\n";
                        pBoard[xx][yy] = pBoard[xx][yy] + pBoard[xx+(1<<i)][yy];
                        pBoard[xx+(1<<i)][yy] = particle();
                        pBoard[xx][yy] = pBoard[xx][yy] + pBoard[xx][yy+(1<<i)];
                        pBoard[xx][yy+(1<<i)] = particle();
                        pBoard[xx][yy] = pBoard[xx][yy] + pBoard[xx+(1<<i)][yy+(1<<i)];
                        pBoard[xx+(1<<i)][yy+(1<<i)] = particle();
                    }
                }
            }
        }
        // Now the only massive particles in the pboard should be the compressed
        // Print it all out
        for(int yy = l - 1; yy>=0; yy--)
        {
            for(int xx=0; xx < l; xx++)
            {
                int m = pBoard[xx][yy].mass, p=0;
                if(m) // If the particle has a mass
                {   
                    temp.masses.push_back(pBoard[xx][yy]);
                    while(m/=2) p++;
                    cout<< p/2 << " ";
                } 
                else if(board[xx][yy].type == 2) cout<<"- ";
                else cout <<"  ";
            }
            cout<<'\n';
        }

        // Now the body, 'temp' has all of the particles inside of it // Sadly I will not program my algorithm to simplify it
        temp.rotate(rotate); // Rotate about the origin
        temp.translate(shift); // Shift it all
        temp.calculateCOM(); // Get the COM of this guy
        cout<<"There are " << temp.masses.size() << " points\n";
        cout << "Mass: " << temp.COM.mass << " with a compression ratio of "<<(float)(temp.masses.size())/(float)temp.mass <<"\n";
        temp.calculateMoI();
        cout << "Moment of Interia: " << temp.moI << '\n';
        bodies.push_back(temp);
    }

        
    f.close();
    return bodies;
}

//void body::rotate(double t, vec axis){}


void body::rotate(double t) // Assume rotation about COM
{
    for(unsigned i = 0; i < polygon.size(); i++)
    {
        polygon[i] = polygon[i] - COM.pos;
        polygon[i] = polygon[i].rotate(t);
        polygon[i] = polygon[i] + COM.pos;
    }
    for(unsigned i=0; i<masses.size(); i++) // For every particle
    {
        masses[i].pos = masses[i].pos - COM.pos;
        masses[i].pos = masses[i].pos.rotate(t);
        masses[i].pos = masses[i].pos + COM.pos;
    }
}

void body::translate(vec r){
    COM.pos = COM.pos + r;
    for(unsigned i = 0; i < polygon.size(); i++)
    {
        polygon[i] = polygon[i] + r;
    }
    for(unsigned i=0; i<masses.size(); i++) // For every particle
    {
        masses[i].pos = masses[i].pos + r;
    }
}

void body::calculateCOM()
{
    particle com(0);
    for(unsigned i=0; i<masses.size(); i++) // For every mass
    {
        com = com + masses[i];
    }
    COM = com; // Set the actual COM to com
    mass = com.mass;
}

void body::calculateMoI()
{
    moI = 0;
    for(unsigned i=0; i<masses.size(); i++)
    {
        moI += masses[i].mass * (mag2(masses[i].pos - COM.pos));
    }
}







/*
// For every horz and vert line that intercepts the line, get the intercept point
            // Get the vertical Intercepts
            vector<vec> vertIntercepts;
            int range = abs(tempLine.v.x), direction = tempLine.v.x/abs(tempLine.v.x); // Get the delta x, and iterate in the direction of the x component
            vertIntercepts.push_back(tempLine.r0); // Have the r0 of the line be the initial intercept
            if(tempLine.v.x != 0)
            {
                for(int j=1; j < range; j++) // Get the intercepts with every vetical line between the the starting point and ending point
                {
                    vertIntercepts.push_back(tempLine.interceptVert(tempLine.v.x + (j*direction)));
                }
            }
            vertIntercepts.push_back(tempLine.r0 + tempLine.v); // Have the ending point of the line be the last intercept



            // Get the Horizontal intercepts
            vector<vec> horzIntercepts;
            range = abs(tempLine.v.y), direction = tempLine.v.y/abs(tempLine.v.y); // Get the delta y, and iterate in the direction of the y component
            horzIntercepts.push_back(tempLine.r0); // Have the r0 of the line be the initial intercept
            if(tempLine.v.y != 0)
            {
                for(int j=1; j < range; j++) // Get the intercepts with every horizontal line between the the starting point and ending point
                {
                    horzIntercepts.push_back(tempLine.interceptVert(tempLine.v.y + (j*direction)));
                }
            }
            horzIntercepts.push_back(tempLine.r0 + tempLine.v); // Have the ending point of the line be the last intercept
// Now we should have every intercept, with the ends of the vectors, be the points which defined the line
            square *currSqr;
            for(int j=0; j < vertIntercepts.size()-1; j++) // For every vertical intercept, except for the last
            {
                if(vertIntercepts[j].y - (int)vertIntercepts[j].y) // If it is not a whole number
                {
                    // Set the respective square in the grid to perimeter and add this line to it (make sure the line does not already exhist)
                    currSqr = &board[vertIntercepts[j].x + axisLength][(int)(vertIntercepts[j].y + axisLength)]; // Point to the current square in the array, to the right
                    if( currSqr->type != 2) // If it is not a perimeter already
                    {
                        currSqr->type = 2; // make it a perimeter
                        if(tempLine.v.x < 0) currSqr->uBound = true; // If the line goes right to left, it is an upper bound
                    }
                    if( currSqr->recentLine != &tempLine) // if this line has not already been added to this square
                    {
                        currSqr->lines.push_back(tempLine); // Add it to the square's list of intersecting lines
                        currSqr->recentLine = &tempLine; // Make the recent line the just added line
                    }

                    currSqr = &board[vertIntercepts[j].x + axisLength - 1][(int)(vertIntercepts[j].y + axisLength)]; // Point to the square left of the interception
                    if( currSqr->type != 2) // If it is not a perimeter already
                    {
                        currSqr->type = 2; // make it a perimeter
                        if(tempLine.v.x < 0) currSqr->uBound = true; // If the line goes right to left, it is an upper bound
                    }
                    if( currSqr->recentLine != &tempLine) // if this line has not already been added to this square
                    {
                        currSqr->lines.push_back(tempLine); // Add it to the square's list of intersecting lines
                        currSqr->recentLine = &tempLine; // Make the recent line the just added line
                    }
                }
                
            }
else // It is a whole number, the intercept is (int, int)
                {
                    // If the next is vert intercept also whole (int, int) (We know it has to be a different point)
                    if(vertIntercepts[j+1].y == (int)vertIntercepts[j+1].y)
                    {
                        if(vertIntercepts[j].y == vertIntercepts[j+1].y) // if the y component is equal (it is a horizontal line)
                        {
                            if(tempLine.v.x > 0) currSqr = &board[vertIntercepts[j].x + axisLength][vertIntercepts[j].y + axisLength]; // Check if left to right
                            else currSqr = &board[vertIntercepts[j].x + axisLength][vertIntercepts[j].y + axisLength - 1]; // or right to left
                            if( currSqr->type != 2) // If it is not a perimeter already
                            {
                                currSqr->type = 2; // make it a perimeter
                                if(tempLine.v.x < 0) currSqr->uBound = true; // If the line goes right to left, it is an upper bound
                            }
                            if( currSqr->recentLine != &tempLine) // if this line has not already been added to this square
                            {
                                currSqr->lines.push_back(tempLine); // Add it to the square's list of intersecting lines
                                currSqr->recentLine = &tempLine; // Make the recent line the just added line
                            }
                        }
                        else if(vertIntercepts[j].y != vertIntercepts[j+1].y)// else, x comp is not equal, both are different and (its a 45 deg line)
                        {
                            int dy = 0, dx = 0;
                            if(tempLine.v.y > 0) dx = -1;
                            if(tempLine.v.x < 0) dy = -1;
                            currSqr = &board[vertIntercepts[j].x + axisLength + dx][vertIntercepts[j].y + axisLength + dy];
                            if( currSqr->type != 2) // If it is not a perimeter already
                            {
                                currSqr->type = 2; // make it a perimeter
                                if(tempLine.v.x < 0) currSqr->uBound = true; // If the line goes right to left, it is an upper bound
                            }
                            if( currSqr->recentLine != &tempLine) // if this line has not already been added to this square
                            {
                                currSqr->lines.push_back(tempLine); // Add it to the square's list of intersecting lines
                                currSqr->recentLine = &tempLine; // Make the recent line the just added line
                            }
                        }
                        else // Else, the next point is a standard intercept and this will be a square with no line
                        {

                        }
                    }
                        
                    // Else, the next point is a standard intercept
                        // if it is an end point, (j=0|size-1) 
                            //continue, for we do not add any suares if this is the case
                        // here we know it is lonely (in the case of a near vert|horz line, we need more significance)
                        // Add the correct square, given thie line direction (shown on paper)
                        
                }
*/