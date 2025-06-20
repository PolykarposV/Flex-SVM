#include <stdio.h>
#include "samples.h"

int samples[125][4] = {
    {12, 15, 0, 4},
    {8, 0, 8, 0},
    {8, 0, 15, 8},
    {8, 4, 15, 15},
    {0, 0, 8, 0},
    {4, 12, 8, 0},
    {4, 8, 8, 12},
    {15, 15, 12, 4},
    {12, 15, 15, 4},
    {12, 4, 12, 15},
    {8, 12, 8, 0},
    {12, 12, 12, 0},
    {4, 4, 15, 4},
    {8, 0, 0, 0},
    {15, 15, 8, 0},
    {4, 4, 12, 8},
    {15, 15, 4, 12},
    {0, 4, 0, 15},
    {0, 0, 4, 15},
    {15, 0, 12, 12},
    {4, 0, 8, 15},
    {0, 15, 0, 8},
    {0, 0, 4, 12},
    {15, 0, 0, 15},
    {8, 8, 4, 15},
    {12, 12, 0, 12},
    {8, 8, 8, 0},
    {4, 0, 0, 8},
    {15, 15, 4, 0},
    {8, 4, 4, 0},
    {0, 8, 0, 12},
    {15, 0, 4, 0},
    {12, 4, 8, 12},
    {12, 4, 15, 8},
    {8, 15, 12, 0},
    {0, 4, 0, 0},
    {8, 4, 12, 15},
    {0, 4, 12, 15},
    {15, 4, 15, 15},
    {8, 15, 15, 4},
    {15, 8, 12, 8},
    {0, 8, 4, 8},
    {4, 15, 15, 15},
    {8, 0, 15, 12},
    {0, 0, 8, 4},
    {8, 8, 0, 15},
    {8, 0, 4, 4},
    {12, 4, 4, 15},
    {4, 8, 8, 8},
    {4, 12, 4, 8},
    {4, 0, 15, 4},
    {8, 12, 15, 12},
    {15, 15, 0, 12},
    {4, 0, 8, 4},
    {0, 4, 15, 0},
    {15, 0, 15, 0},
    {0, 0, 0, 8},
    {8, 4, 12, 12},
    {4, 4, 0, 15},
    {12, 4, 12, 8},
    {4, 0, 15, 15},
    {15, 15, 0, 8},
    {0, 4, 12, 8},
    {15, 8, 4, 0},
    {4, 4, 15, 8},
    {15, 15, 12, 15},
    {8, 12, 15, 4},
    {15, 12, 4, 15},
    {4, 8, 12, 12},
    {15, 15, 8, 8},
    {8, 12, 0, 8},
    {8, 0, 4, 15},
    {4, 8, 0, 0},
    {4, 4, 8, 4},
    {12, 8, 15, 12},
    {8, 8, 0, 0},
    {8, 4, 15, 4},
    {15, 8, 15, 0},
    {12, 4, 0, 0},
    {15, 4, 4, 8},
    {8, 8, 0, 4},
    {12, 8, 15, 15},
    {8, 4, 0, 15},
    {0, 8, 12, 4},
    {12, 0, 4, 15},
    {8, 12, 15, 15},
    {12, 15, 12, 15},
    {4, 4, 12, 12},
    {4, 8, 15, 12},
    {15, 8, 4, 15},
    {8, 15, 15, 12},
    {15, 4, 4, 12},
    {8, 15, 15, 0},
    {15, 0, 15, 15},
    {4, 15, 12, 0},
    {8, 0, 15, 0},
    {8, 0, 4, 8},
    {8, 15, 12, 4},
    {12, 4, 4, 8},
    {15, 12, 0, 4},
    {12, 0, 8, 8},
    {8, 15, 0, 0},
    {8, 15, 8, 0},
    {12, 0, 8, 15},
    {4, 4, 12, 4},
    {12, 12, 0, 8},
    {8, 15, 8, 4},
    {15, 8, 12, 12},
    {8, 12, 4, 8},
    {15, 12, 12, 8},
    {15, 0, 15, 4},
    {8, 15, 15, 8},
    {0, 0, 0, 15},
    {12, 12, 12, 8},
    {15, 12, 0, 15},
    {8, 8, 8, 15},
    {4, 15, 12, 4},
    {15, 4, 15, 8},
    {4, 4, 0, 4},
    {0, 15, 15, 8},
    {0, 4, 8, 4},
    {4, 4, 4, 15},
    {0, 8, 12, 0},
    {4, 0, 0, 12},
    {8, 15, 8, 8}
};
