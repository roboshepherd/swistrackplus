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
    {950, 840}, // task 1 start point
    {1797, 713},
    {1848, 1713},
    {535, 1596}
};


#endif
