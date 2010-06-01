#ifndef HEADER_RILObjects
#define HEADER_RILObjects

#include "RILSetup.h"

//RIL and AFM params
#ifdef MAXSHOPTASK
#undef MAXSHOPTASK
#endif

#define MAXSHOPTASK 4

static const char* RILROBOTLIST[] = {
"1"
};

// test setup
static float TASKS_CENTERS[MAXSHOPTASK][XY] = {
    {2344, 960}, // task 1 start point
    {2891, 1840},
    {1699, 1864},
    {1558, 730}
};


#endif
