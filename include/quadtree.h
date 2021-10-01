#pragma once

#include "particle.h"

class quadtree{
    public:
        quadtree *tree[2][2], *parent; // Which tree does this current tree come from
        bool split; // Identify whether or not has sub nodes
        particle com; // The center of mass
        quadtree();
};