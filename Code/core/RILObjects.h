#ifndef HEADER_RILObjects
#define HEADER_RILObjects

#include "RILSetup.h"

static const char* RILROBOTLIST[] = {
  EPUCK1246,
  EPUCK1250,
  EPUCK1253,
  EPUCK1259,
  EPUCK1260,
  EPUCK1265,
  EPUCK1271,
  EPUCK1302
};

// test setup
static float TASKS_CENTERS[MAXSHOPTASK][XY] = {
    {354, 2327}, // task 1 start point
    {1578, 310},
    {2873, 1986}
};


#endif
