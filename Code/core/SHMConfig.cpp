#include "SHMConfig.h"
#define THISCLASS SHMConfig

void THISCLASS::RemoveSHM(eSHMType targetSHM)
{
    if(targetSHM == ePoseSHM){

        struct shm_remove{
         shm_remove() { shared_memory_object::remove(POSE_SHM); }
         ~shm_remove(){ shared_memory_object::remove(POSE_SHM); }
        } remover;

        printf("POSE_SHM removed\n");

    }  else if(targetSHM == eStateSHM){

        struct shm_remove{
         shm_remove() { shared_memory_object::remove(STATE_SHM); }
         ~shm_remove(){ shared_memory_object::remove(STATE_SHM); }
        } remover;

        printf("STATE_SHM removed\n");

    } else if(targetSHM == eTaskSHM){

        struct shm_remove{
         shm_remove() { shared_memory_object::remove(TASK_SHM); }
         ~shm_remove(){ shared_memory_object::remove(TASK_SHM); }
        } remover;

        printf("TASK_SHM removed\n");

    } else if(targetSHM == eNavCmdSHM){

        struct shm_remove{
         shm_remove() { shared_memory_object::remove(NAVCMD_SHM); }
         ~shm_remove(){ shared_memory_object::remove(NAVCMD_SHM); }
        } remover;

        printf("NAVCMD_SHM removed\n");

    } else if(targetSHM == eNavEvtSHM){

        struct shm_remove{
         shm_remove() { shared_memory_object::remove(NAVEVT_SHM); }
         ~shm_remove(){ shared_memory_object::remove(NAVEVT_SHM); }
        } remover;

        printf("NAVEVT_SHM removed\n");
    } else if(targetSHM == eBroadcastSHM){

        struct shm_remove{
         shm_remove() { shared_memory_object::remove(BROADCAST_SHM); }
         ~shm_remove(){ shared_memory_object::remove(BROADCAST_SHM); }
        } remover;

        printf("BROADCAST_SHM removed\n");
    } else {

        printf("Unknown SHM type\n");
    }

}


void THISCLASS::InitPoseSHM( const char* robotlist[])
{
    PoseBuffer *buffer[MAXROBOT];

      //Construct managed shared memory
      managed_shared_memory segment(
        open_or_create,
        POSE_SHM,
        SHM_SZ * sizeof(PoseBuffer));


      //Create all object of shared_memory_buffer
      for(int i = 0; i < MAXROBOT; ++i){
        (buffer)[i] = segment.construct<PoseBuffer> (robotlist[i]) ();
      }

    printf("Created Pose Objects: %d\n", (int)segment.get_num_named_objects());
}

void THISCLASS::InitStateSHM( const char* robotlist[])
{

    StateBuffer *buffer[MAXROBOT];

      //Construct managed shared memory
      managed_shared_memory segment(
        open_or_create,
        STATE_SHM,
        SHM_SZ * sizeof(StateBuffer));


      //Create all object of shared_memory_buffer
      for(int i = 0; i < MAXROBOT; ++i){
        buffer[i] = segment.construct<StateBuffer> (robotlist[i]) ();
      }

    printf("Created State Objects: %d\n", (int)segment.get_num_named_objects());
}


void THISCLASS::InitTaskSHM( const char* robotlist[])
{

    TaskBuffer *buffer[MAXROBOT];

      //Construct managed shared memory
      managed_shared_memory segment(
        open_or_create,
        TASK_SHM,
        SHM_SZ * sizeof(TaskBuffer));


      //Create all object of shared_memory_buffer
      for(int i = 0; i < MAXROBOT; ++i){
        buffer[i] = segment.construct<TaskBuffer> (robotlist[i]) ();
      }

    printf("Created Task Objects: %d\n", (int)segment.get_num_named_objects());
}

void THISCLASS::InitNavCmdSHM( const char* robotlist[])
{
    NavCmdBuffer *buffer[MAXROBOT];

      //Construct managed shared memory
      managed_shared_memory segment(
        open_or_create,
        NAVCMD_SHM,
        SHM_SZ * sizeof(NavCmdBuffer));


      //Create all object of shared_memory_buffer
      for(int i = 0; i < MAXROBOT; ++i){
        buffer[i] = segment.construct<NavCmdBuffer> (robotlist[i]) ();
      }

    printf("Created NavCmd Objects: %d\n", (int)segment.get_num_named_objects());
}

void THISCLASS::InitNavEvtSHM( const char* robotlist[])
{

    NavEvtBuffer *buffer[MAXROBOT];

      //Construct managed shared memory
      managed_shared_memory segment(
        open_or_create,
        NAVEVT_SHM,
        SHM_SZ * sizeof(NavEvtBuffer));


      //Create all object of shared_memory_buffer
      for(int i = 0; i < MAXROBOT; ++i){
        buffer[i] = segment.construct<NavEvtBuffer> (robotlist[i]) ();
      }

    printf("Created NavEvt Objects: %d\n", (int)segment.get_num_named_objects());
}


void THISCLASS::InitBroadcastSHM(const char* robotlist[])
{
  BroadcastBuffer *buffer[MAXROBOT];

      //Construct managed shared memory
      managed_shared_memory segment(
        open_or_create,
        BROADCAST_SHM,
        SHM_SZ * sizeof(BroadcastBuffer));


      //Create all object of shared_memory_buffer
      for(int i = 0; i < MAXROBOT; ++i){
        buffer[i] = segment.construct<BroadcastBuffer> (robotlist[i]) ();
      }

    printf("Created TaskBroadcast Objects: %d\n", (int)segment.get_num_named_objects());
}
void THISCLASS::InitPeersSHM(const char* robotlist[])
{
}
void THISCLASS::InitTaskEstimateSHM(const char* robotlist[])
{
}


// Find Object

void THISCLASS::FindObject(eSHMType targetSHM, const char* objid)
{
    //printf("SHM type given: %d  poseSHM: %d\n", (int) targetSHM, (int) ePoseSHM);
    if(targetSHM == ePoseSHM){
        //type
        PoseBufferMapType posedata;
        printf("Opening POSE_SHM\n");
        // open SHM
        managed_shared_memory segment(
        open_only,
        POSE_SHM);

        // find object
         posedata = segment.find<PoseBuffer> (objid);

         if(posedata.second == 1)
            printf("PoseBuffer Object for object %s found \n", objid);


    } else if (targetSHM == eStateSHM) {
        //type
        StateBufferMapType statedata;

        // open SHM
        managed_shared_memory segment(
        open_only,
        STATE_SHM);

        // find object
         statedata = segment.find<StateBuffer> (objid);

         if(statedata.second == 1)
            printf("StateBuffer Object found \n");

    } else if (targetSHM == eTaskSHM) {
        //type
        TaskBufferMapType taskdata;

        // open SHM
        managed_shared_memory segment(
        open_only,
        TASK_SHM);

        // find object
         taskdata = segment.find<TaskBuffer> (objid);

         if(taskdata.second == 1)
            printf("StateBuffer Object found \n");


    } else if (targetSHM == eNavCmdSHM) {
        //type
        NavCmdBufferMapType navcmddata;

        // open SHM
        managed_shared_memory segment(
        open_only,
        NAVCMD_SHM);

        // find object
         navcmddata = segment.find<NavCmdBuffer> (objid);

         if(navcmddata.second == 1)
            printf("NavCmdBuffer Object found \n");

    } else if (targetSHM == eNavEvtSHM) {
                //type
        NavEvtBufferMapType navevtdata;

        // open SHM
        managed_shared_memory segment(
        open_only,
        NAVEVT_SHM);

        // find object
         navevtdata = segment.find<NavEvtBuffer> (objid);

         if(navevtdata.second == 1)
            printf("NavEvtBuffer Object found \n");
    } else if (targetSHM == eBroadcastSHM) {
                //type
        BroadcastBufferMapType data;

        // open SHM
        managed_shared_memory segment(
        open_only,
        BROADCAST_SHM);

        // find object
         data = segment.find<BroadcastBuffer> (objid);

         if(data.second == 1)
            printf("BroadcastBuffer Object found \n");
    } else {
        printf("Unknown SHM type\n");
    }
}



void THISCLASS::CleanupSHM(eSHMType targetSHM, const char* robotlist[])
{
    if(targetSHM == ePoseSHM){
        // open SHM
        managed_shared_memory segment(
        open_only,
        POSE_SHM);

        //destroy all objects
        for(int i = 0; i < MAXROBOT; ++i){
            segment.destroy<PoseBuffer> (robotlist[i]);
        }

        printf("All pose objects destroyed\n");
    } else if(targetSHM == eStateSHM){
        // open SHM
        managed_shared_memory segment(
        open_only,
        STATE_SHM);

        //destroy all objects
        for(int i = 0; i < MAXROBOT; ++i){
            segment.destroy<StateBuffer> (robotlist[i]);
        }

        printf("All STATE objects destroyed\n");
    } else if(targetSHM == eTaskSHM){
        // open SHM
        managed_shared_memory segment(
        open_only,
        TASK_SHM);

        //destroy all objects
        for(int i = 0; i < MAXROBOT; ++i){
            segment.destroy<TaskBuffer> (robotlist[i]);
        }

        printf("All TASK objects destroyed\n");
    } else if(targetSHM == eNavCmdSHM){
        // open SHM
        managed_shared_memory segment(
        open_only,
        NAVCMD_SHM);

        //destroy all objects
        for(int i = 0; i < MAXROBOT; ++i){
            segment.destroy<NavCmdBuffer> (robotlist[i]);
        }

        printf("All NAVCMD objects destroyed\n");
    } else if(targetSHM == eNavEvtSHM){
        // open SHM
        managed_shared_memory segment(
        open_only,
        NAVEVT_SHM);

        //destroy all objects
        for(int i = 0; i < MAXROBOT; ++i){
            segment.destroy<NavEvtBuffer> (robotlist[i]);
        }

        printf("All NAVEVT objects destroyed\n");
    } else if(targetSHM == eBroadcastSHM){
        // open SHM
        managed_shared_memory segment(
        open_only,
        BROADCAST_SHM);

        //destroy all objects
        for(int i = 0; i < MAXROBOT; ++i){
            segment.destroy<BroadcastBuffer> (robotlist[i]);
        }

        printf("All BROADCAST objects destroyed\n");
    } else {
        printf("Unknown  SHM type \n");
    }

}


void THISCLASS::CommitPose(const char* objid, RobotDevice::tPose pose){

    PoseBufferMapType posedata;
     //open POSE_SHM
    printf("Committing to POSE_SHM\n");
    // open SHM
    managed_shared_memory segment(
    open_only,
    POSE_SHM);

    // find object
     posedata = segment.find<PoseBuffer> (objid);

    // write data
    PoseBuffer* b = posedata.first;
    b->mutex.wait();
    b->pose = pose;
    ++(b->count);
    b->mutex.post();

    printf("@SHM: Robot %s PoseWritten: %.0f %.0f %.2f\n", objid, b->pose.center.x,\
      b->pose.center.y, b->pose.orient);

}

RobotDevice::tPose THISCLASS::CheckoutPose(const char* objid){

    RobotDevice::tPose pose;
    PoseBufferMapType posedata;
     //open POSE_SHM
    printf("Checking out from POSE_SHM\n");
    // open SHM
    managed_shared_memory segment(
    open_only,
    POSE_SHM);

    // find object
    posedata = segment.find<PoseBuffer> (objid);

    // write data
    PoseBuffer* b = posedata.first;
    b->mutex.wait();
    pose =  b->pose ;
    ++(b->count);
    b->mutex.post();

    printf("@SHM: Robot %s PoseFound: %.0f %.0f %.2f\n", objid, pose.center.x,\
      pose.center.y, pose.orient);

    return pose;
}


void THISCLASS::CommitState(const char* objid, RobotDevice::eState state){

    StateBufferMapType statedata;

    printf("Committing to STATE_SHM\n");
    // open SHM
    managed_shared_memory segment(
    open_only,
    STATE_SHM);

    // find object
     statedata = segment.find<StateBuffer> (objid);

    // write data
    StateBuffer* b = statedata.first;
    b->mutex.wait();
    b->state = state;
    ++(b->count);
    b->mutex.post();
}

RobotDevice::eState THISCLASS::CheckoutState(const char* objid){

    RobotDevice::eState  state;
    StateBufferMapType statedata;

    printf("Checking out from STATE_SHM\n");
    // open SHM
    managed_shared_memory segment(
    open_only,
    STATE_SHM);

    // find object
    statedata = segment.find<StateBuffer> (objid);

    // write data
    StateBuffer* b = statedata.first;
    b->mutex.wait();
    state = b->state;
    ++(b->count);
    b->mutex.post();

    return state;
}

void THISCLASS::CommitTask(const char* objid, int task){

    TaskBufferMapType taskdata;

    printf("Committing to TASK_SHM\n");
    // open SHM
    managed_shared_memory segment(
    open_only,
    TASK_SHM);

    // find object
     taskdata = segment.find<TaskBuffer> (objid);

    // access data
    TaskBuffer* b = taskdata.first;
    b->mutex.wait();
    b->task = task;
    ++(b->count);
    b->mutex.post();
}

int THISCLASS::CheckoutTask(const char* objid){
    int task = -1;
    TaskBufferMapType taskdata;
    printf("Checking out from TASK_SHM\n");
    // open SHM
    managed_shared_memory segment(
    open_only,
    TASK_SHM);

    // find object
     taskdata = segment.find<TaskBuffer> (objid);

    // access data
    TaskBuffer* b = taskdata.first;
    b->mutex.wait();
    task = b->task;
    ++(b->count);
    b->mutex.post();
    return task;
}

void THISCLASS::CommitNavCmd(const char* objid, std::string navcmd){

    NavCmdBufferMapType navcmddata;

    printf("Committing to NAVCMD_SHM\n");
    // open SHM
    managed_shared_memory segment(
    open_only,
    NAVCMD_SHM);

    // find object
     navcmddata = segment.find<NavCmdBuffer> (objid);

    // access data
    NavCmdBuffer* b = navcmddata.first;
    b->mutex.wait();
    b->navcmd = navcmd;
    ++(b->count);
    b->mutex.post();
}

std::string THISCLASS::CheckoutNavCmd(const char* objid){

    std::string navcmd;
    NavCmdBufferMapType navcmddata;

    printf("Checking out from NAVCMD_SHM\n");
    // open SHM
    managed_shared_memory segment(
    open_only,
    NAVCMD_SHM);

    // find object
     navcmddata = segment.find<NavCmdBuffer> (objid);

    // access data
    NavCmdBuffer* b = navcmddata.first;
    b->mutex.wait();
    navcmd = b->navcmd;
    ++(b->count);
    b->mutex.post();

    return navcmd;
}

void THISCLASS::CommitNavEvt(const char* objid, std::string navevt){

    NavEvtBufferMapType navevtdata;

    printf("Committing to NAVEVT_SHM\n");
    // open SHM
    managed_shared_memory segment(
    open_only,
    NAVEVT_SHM);

    // find object
     navevtdata = segment.find<NavEvtBuffer> (objid);

    // access data
    NavEvtBuffer* b = navevtdata.first;
    b->mutex.wait();
    b->navevt = navevt;
    ++(b->count);
    b->mutex.post();

}

std::string THISCLASS::CheckoutNavEvt(const char* objid){

    std::string navevt;
    NavEvtBufferMapType navevtdata;

    printf("Checking out from NAVEVT_SHM\n");
    // open SHM
    managed_shared_memory segment(
    open_only,
    NAVEVT_SHM);

    // find object
     navevtdata = segment.find<NavEvtBuffer> (objid);

    // access data
    NavEvtBuffer* b = navevtdata.first;
    b->mutex.wait();
    navevt = b->navevt;
    ++(b->count);
    b->mutex.post();

    return navevt;
}

//void THISCLASS::CommitTaskBroadcast(const char* objid,\
//  BroadcastBuffer::TaskBroadcastVector tasks)
//{
//    BroadcastBufferMapType data;
//
//    printf("Committing to BROADCAST_SHM\n");
//    // open SHM
//    managed_shared_memory segment(
//    open_only,
//    BROADCAST_SHM);
//
//    // find object
//     data = segment.find<BroadcastBuffer> (objid);
//
//    // access data
//    BroadcastBuffer* b = data.first;
//    b->mutex.wait();
//    b->tasks = tasks;
//    ++(b->count);
//    b->mutex.post();
//
//}
//BroadcastBuffer::TaskBroadcastVector THISCLASS::CheckoutTaskBroadcast(\
//  const char* objid)
//{
//    BroadcastBuffer::TaskBroadcastVector tasks;
//
//    BroadcastBufferMapType data;
//
//    printf("Committing to BROADCAST_SHM\n");
//    // open SHM
//    managed_shared_memory segment(
//    open_only,
//    BROADCAST_SHM);
//
//    // find object
//     data = segment.find<BroadcastBuffer> (objid);
//
//    // access data
//    BroadcastBuffer* b = data.first;
//    b->mutex.wait();
//    tasks = b->tasks;
//    ++(b->count);
//    b->mutex.post();
//
//    return tasks;
//}


/*  ---------- API v2 ---------  */
void THISCLASS::CommitPoseMessage(const char* objid, PoseMessageType msg)
{
    PoseBufferMapType posedata;
     //open POSE_SHM
    //printf("Committing to POSE_SHM\n");
    // open SHM
    managed_shared_memory segment(
    open_only,
    POSE_SHM);

    // find object
     posedata = segment.find<PoseBuffer> (objid);

    // write data
    PoseBuffer* b = posedata.first;
    b->mutex.wait();
    b->pose = msg.pose;
    b->count = msg.step;
    b->mutex.post();

//    printf("@SHM: [Step %ld] Robot %s PoseWritten: %.0f %.0f %.2f\n",\
//     b->count, objid, b->pose.center.x, b->pose.center.y, b->pose.orient);
}

PoseMessageType THISCLASS::CheckoutPoseMessage(const char* objid)
{
    PoseMessageType msg;
    PoseBufferMapType posedata;
     //open POSE_SHM
    //printf("Checking out from POSE_SHM\n");
    // open SHM
    managed_shared_memory segment(
    open_only,
    POSE_SHM);

    // find object
    posedata = segment.find<PoseBuffer> (objid);

    // write data
    PoseBuffer* b = posedata.first;
    b->mutex.wait();
    msg.pose =  b->pose ;
    msg.step= b->count;
    b->mutex.post();

//    printf("@SHM: [Step %ld] Robot %s PoseFound: %.0f %.0f %.2f\n",\
//     b->count, objid, b->pose.center.x, b->pose.center.y, b->pose.orient);

    return msg;
}

void THISCLASS::CommitStateMessage(const char* objid, StateMessageType msg)
{

    StateBufferMapType statedata;

    //printf("Committing to STATE_SHM\n");
    // open SHM
    managed_shared_memory segment(
    open_only,
    STATE_SHM);

    // find object
     statedata = segment.find<StateBuffer> (objid);

    // access data
    StateBuffer* b = statedata.first;
    b->mutex.wait();
    b->state = msg.state;
    b->count = msg.step;
    b->mutex.post();
}

StateMessageType THISCLASS::CheckoutStateMessage(const char* objid)
{
    StateMessageType msg;
    StateBufferMapType statedata;
    //printf("Checking out from STATE_SHM\n");
    // open SHM
    managed_shared_memory segment(
    open_only,
    STATE_SHM);

    // find object
     statedata = segment.find<StateBuffer> (objid);

    // access data
    StateBuffer* b = statedata.first;
    b->mutex.wait();
    msg.state = b->state;
    msg.step = b->count;
    b->mutex.post();
    return msg;
}

void THISCLASS::CommitTaskBroadcastMessage(const char* objid,\
  TaskBroadcastMessageType msg)
{

    //BroadcastBuffer::TaskBroadcastVector::iterator it = msg.tasks.begin();
    BroadcastBufferMapType data;

    //printf("Committing to BROADCAST_SHM\n");
    // open SHM
    managed_shared_memory segment(
    open_only,
    BROADCAST_SHM);

    // find object
     data = segment.find<BroadcastBuffer> (objid);

    // access data
    BroadcastBuffer* b = data.first;
    b->mutex.wait();
//    while (it != msg.tasks.end()) {
//      printf("task urgency val is now %f\n", (*it).urgency);
//      b->tasks.push_back(*it);
//      it++;
//    }
    for (int i = 0; i < MAXSHOPTASK; i++) {
      b->tasks[i] = msg.tasks[i];
    }

    b->count = msg.step;
    b->mutex.post();
}

TaskBroadcastMessageType THISCLASS::CheckoutTaskBroadcastMessage(const char* objid)
{
    TaskBroadcastMessageType msg;
    BroadcastBufferMapType data;
     BroadcastBuffer::tTaskBroadcast tb;
    //printf("Checkout from BROADCAST_SHM\n");
    // open SHM
    managed_shared_memory segment(
    open_only,
    BROADCAST_SHM);

    // find object
     data = segment.find<BroadcastBuffer> (objid);

    // access data
    BroadcastBuffer* b = data.first;
//    BroadcastBuffer::TaskBroadcastVector::iterator it = b->tasks.begin();
    b->mutex.wait();
//    while (it != b->tasks.end()) {
//      tb.id = (*it).id;
//      tb.center = (*it).center;
//      tb.urgency = (*it).urgency;
//      printf("task urgency val is now %f\n", tb.urgency);
//      msg.tasks.push_back(tb);
//      it++;
//    }

    for (int i = 0; i < MAXSHOPTASK; i++) {
      msg.tasks[i]= b->tasks[i];
    }
    msg.step = b->count;
    b->mutex.post();

    return msg;
}
