#ifndef Header_SHMConfig
#define Header_SHMConfig

#include <vector>
#include <string.h>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/sync/interprocess_semaphore.hpp>
using namespace boost::interprocess;

// Names of All SHM
#define POSE_SHM  "RobotPoseTable" // written by Central/tracker loop
#define STATE_SHM "RobotStateReport" // written by client
#define TASK_SHM  "TaskAllocationTable" // written by client

#define NAVCMD_SHM "NavigantionCommandTable" // written by client
#define NAVEVT_SHM "RobotNavigationEventReport" // written by client

#define BROADCAST_SHM "GlobalTaskBroadcast"
#define PEERS_SHM "LocalPeerList"
#define TASK_ESTIMATE_SHM "LocalTaskEstimates"

#define SHM_COUNT 8

#include "RILSetup.h"

#define UNIT_SZ 4 * 65536
#define SHM_SZ (MAXROBOT * UNIT_SZ)



#include "RobotDevice.h"

// Index Key for all SHM
enum eSHMType {
        ePoseSHM = 1,
        eStateSHM, // robots' device states
        eTaskSHM, // robots' assigned tasks
        eBroadcastSHM, // robots' task broadcast info
        ePeersSHM, // list of neighbours for each robot
        eTaskEstimateSHM, // robots' estimates of task location/workers
        eNavCmdSHM,
        eNavEvtSHM,
};



// SHM Buffer data structures

class BroadcastBuffer
{

   public:
    BroadcastBuffer():\
     mutex(1), tasks(), count(0) {}

    //Semaphores to protect and synchronize access
    interprocess_semaphore mutex;

    //Items to fill
    typedef struct TaskBroadcast {
      TaskBroadcast(): id(-1), center(), urgency(){}
      int id;
      CvPoint2D32f center;
      double urgency;
    } tTaskBroadcast;
    //typedef std::vector<tTaskBroadcast> TaskBroadcastVector;
    //TaskBroadcastVector tasks;
    tTaskBroadcast tasks[MAXSHOPTASK];
    int count; // write access count
};


class PeersBuffer
{

   public:
    PeersBuffer():\
     mutex(1), peers(), count(0) {}

    //Semaphores to protect and synchronize access
    interprocess_semaphore mutex;

    //Items to fill
    typedef std::vector<int> PeersVector;
    PeersVector peers;
    int count; // write access count
};

class TaskEstimateBuffer
{

   public:
    TaskEstimateBuffer():\
     mutex(1), task_estimates(), count(0) {}

    //Semaphores to protect and synchronize access
    interprocess_semaphore mutex;

    //Items to fill
    typedef struct TaskEstimate {
      TaskEstimate(): start(), end(),  workers(){}
      CvPoint2D32f start; // by looking for color patches on floor
      CvPoint2D32f end;
      int workers; // circling around the task and counting others' presence
    } tTaskEstimate;
    typedef std::vector<tTaskEstimate> TaskEstimateVector;
    TaskEstimateVector task_estimates;

    int count; // write access count
};

class PoseBuffer
{

   public:
    PoseBuffer():\
     mutex(1), pose(),count(0){}

    //Semaphores to protect and synchronize access
    interprocess_semaphore mutex;

    //Items to fill
    RobotDevice::tPose pose; // robot state
    int count; // write access count
};


class StateBuffer
{

   public:
    StateBuffer():\
     mutex(1), state(RobotDevice::UNAVAILABLE),\
      count(0){}

    //Semaphores to protect and synchronize access
    interprocess_semaphore mutex;

    //Items to fill
    RobotDevice::eState state; // robot state
    int count; // write access count
};


class TaskBuffer
{

   public:
    TaskBuffer():\
     mutex(1), task(-1),\
      count(0){}

    //Semaphores to protect and synchronize access
    interprocess_semaphore mutex;

    //Items to fill
    int task; // robot task TODO: should use ShopTask
    int count; // write access count
};


class NavCmdBuffer
{

   public:
    NavCmdBuffer():\
     mutex(1), navcmd(""),\
      count(0){}

    //Semaphores to protect and synchronize access
    interprocess_semaphore mutex;

    //Items to fill
    std::string navcmd; //TODO: Use NavigationCommand
    int count; // write access count
};


class NavEvtBuffer
{

   public:
    NavEvtBuffer():\
     mutex(1), navevt(""),\
      count(0){}

    //Semaphores to protect and synchronize access
    interprocess_semaphore mutex;

    //Items to fill
     std::string navevt; //TODO: Use NavigationEvent
    int count; // write access count
};


// SHM data message type -- v2 API
typedef struct PoseMessage {
  RobotDevice::tPose pose;
  long step;
} PoseMessageType;

typedef struct TaskMessage {
  int task;
  long step;
} TaskMessageType;

typedef struct StateMessage {
  RobotDevice::eState state;
  long step;
} StateMessageType;


typedef struct TaskBroadcastMessage {
  //BroadcastBuffer::TaskBroadcastVector tasks;
  BroadcastBuffer::tTaskBroadcast tasks[MAXSHOPTASK];
  long step;
} TaskBroadcastMessageType;


// TODO for peers and task estimates



// Maps for finding SHM buffer types
typedef std::pair<PoseBuffer*, std::size_t> PoseBufferMapType;
typedef std::pair<StateBuffer*, std::size_t> StateBufferMapType;
typedef std::pair<TaskBuffer*, std::size_t> TaskBufferMapType;
typedef std::pair<NavCmdBuffer*, std::size_t> NavCmdBufferMapType;
typedef std::pair<NavEvtBuffer*, std::size_t> NavEvtBufferMapType;

typedef std::pair<BroadcastBuffer*, std::size_t> BroadcastBufferMapType;
typedef std::pair<PeersBuffer*, std::size_t> PeersBufferMapType;
typedef std::pair<TaskEstimateBuffer*, std::size_t> TaskEstimateBufferMapType;


class SHMConfig
{
  public:

    //! Remove a SHM, (if exists), **in beginning and end of all operations**
    void RemoveSHM(eSHMType targetSHM);

    //! Open or Create a SHM block -- mainly used by ControllerMainLoop
    void InitPoseSHM(const char* robotlist[]);
    void InitStateSHM(const char* robotlist[]);
    void InitTaskSHM(const char* robotlist[]);
    void InitNavCmdSHM(const char* robotlist[]);
    void InitNavEvtSHM(const char* robotlist[]);

    void InitBroadcastSHM(const char* robotlist[]);
    void InitPeersSHM(const char* robotlist[]);
    void InitTaskEstimateSHM(const char* robotlist[]);

    //! Individual client can search for their own object with objid
    void FindObject(eSHMType targetSHM, const char* objid);


    //! Commit/Checkout operations used by clients and ControllerMainLoop

    void CommitPose(const char* objid, RobotDevice::tPose pose);
    RobotDevice::tPose CheckoutPose(const char* objid);

    void CommitState(const char* objid, RobotDevice::eState state);
    RobotDevice::eState CheckoutState(const char* objid);

    void CommitTask(const char* objid, int task);
    int CheckoutTask(const char* objid);

    void CommitNavCmd(const char* objid, std::string navcmd);
    std::string CheckoutNavCmd(const char* objid);

    void CommitNavEvt(const char* objid, std::string navevt);
    std::string CheckoutNavEvt(const char* objid);

//    void CommitTaskBroadcast(const char* objid,\
//      BroadcastBuffer::tTaskBroadcast tasks[]);
//    BroadcastBuffer::tTaskBroadcast* CheckoutTaskBroadcast(const char* objid);

    void CommitPeers(const char* objid,\
      PeersBuffer::PeersVector peers);
    PeersBuffer::PeersVector CheckoutPeers(const char* objid);

    void CommitTaskEstimate(const char* objid,\
      TaskEstimateBuffer::TaskEstimateVector task_estimates);
    TaskEstimateBuffer::TaskEstimateVector CheckoutTaskEstimate(const char* objid);


    // API v2
    //--------------
    void CommitPoseMessage(const char* objid, PoseMessageType msg);
    PoseMessageType CheckoutPoseMessage(const char* objid);

    void CommitStateMessage(const char* objid, StateMessageType msg);
    StateMessageType CheckoutStateMessage(const char* objid);

    void CommitTaskBroadcastMessage(const char* objid,\
      TaskBroadcastMessageType msg);
   TaskBroadcastMessageType CheckoutTaskBroadcastMessage(const char* objid);

    //----
    // TODO for peers and task estimates


    //! Object Clean up
    void CleanupSHM(eSHMType targetSHM, const char* robotlist[]);



    //! Constructor
    SHMConfig(){}

    //! Destructor
    ~SHMConfig(){}


};

#endif
