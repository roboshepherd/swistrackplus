#ifndef Header_RILSetup
#define Header_RILSetup

// Size
#define MAXROBOT 8

// RIL robot id alias -- ids are same in SwisTracker
#define EPUCK1246 "1"
#define EPUCK1250 "2"
#define EPUCK1253 "3"
#define EPUCK1259 "4"
#define EPUCK1260 "5"
#define EPUCK1265 "6"
#define EPUCK1271 "7"
#define EPUCK1302 "8"


//RIL and AFM params
#define MAXSHOPTASK 3
#define INIT_URGENCY 0.5
#define DLETA_URGENCY 0.01
#define INIT_MATERIAL_COUNT 10
#define XY 2 // for task coordinates

//robot device
#define INIT_SENSITIZATION 1
#define INIT_LEARN_RATE 1
#define INIT_FORGET_RATE 0.16


// Names of SHM objects
//static const char* RILROBOTLIST[] = {
//  EPUCK1246,
//  EPUCK1250,
//  EPUCK1253,
//  EPUCK1259,
//  EPUCK1260,
//  EPUCK1265,
//  EPUCK1271,
//  EPUCK1302
//};





//// fake
//static float TASKS_START[MAXSHOPTASK][XY] = {
//    {100, 500}, // task 1 start point
//    {700, 100},
//    {1500, 500}
//};
//
//static float TASKS_END[MAXSHOPTASK][XY] = {
//    {400, 800}, // task 1 end point
//    {1200, 300},
//    {1800, 700}
//};
//
//static float STORE_LOCATION[XY] = {800, 1600};
//

#endif
