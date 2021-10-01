# JGraphity
A gravity sim using my professor's graphing library

USE: (To generate all of 5 of the examples)
  make
  make run

FILE FORMAT:
  - First Line
      - "AxisPower: 3"
        - This will specify, what power of 2 long is the axis from 0?
        - This will limit the range you will be able to draw/make the polygon in
  - Body Format
      - "NumVerticies: 4"
        - This is where you specify how many verticies the body/polygon you want to make has
      - "Verticies: 2 -1 -2 -1 -2 -2 2 -2"
        - This line you list off the coordinates of all of the verticies
          - x1 y1 x2 y2 x3 y3 x4 y4
        - The coordinates have to be within [-2^3, 2^3]
      - "Rotate: 10"
        - From the orgin of you polygon drawing, how much do you want to rotate
        - In degrees (Pos is CCW)
      - "Shift: 1 2"
        - This is how we will move the bodies to have different starting positions in space
        
RUNNING ./grav:
  - What it does:
      - This generates the .jgr file, which jgraph will interpret and plot
  - Usage:
      - ./grav [body file] [time step size] [steps per frame] [number of steps]  
        -  [body file]
            - This is where you specify the .txt file that declares the starting conditions of the simulation
        -  [time step size]
            - This is where you specify the time between every step in the simulation
        -  [steps per frame]
            - This is where you specify the number of steps between every frame or "snapshot" of the simulation 
        -  [number of steps]
            - This is where you specify the number of steps you would like the progran to take athe the current timesteop
            - This also affects how many "Frames" you will get

GENERATING THE GRAPH:
- Given file name f.txt which you ran the sim on
  - Execute:
    - ./j -P f.jgr | ps2pdf - | convert -density 300 - -quality 100 f.jpg 

OUTPUT:
- While the program runs
    - It will output the generation and compression of each polygon
        - The commpression was my own idea, bassically that you can treat large, uninteresting regions, as more massive point particles
        - The generation has more room to be made more accurate, using a sort of anti-aliasing which I had spent too much time planning how to implement  

Intentions:
- I had planned to implement the barns-hutt approximation for a speed boost, but that would have taken a lot more planning and testing of possible implementations
