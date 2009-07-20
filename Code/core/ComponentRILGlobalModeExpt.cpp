#include "ComponentRILGlobalModeExpt.h"
#define THISCLASS ComponentRILGlobalModeExpt

// shortcuts
#define MC_DS_ROBOTDEVICES mCore->mDataStructureRobotDevices.mRobotDevices
#define MC_DS_SHOPTASKS mCore->mDataStructureShopTasks.mShopTasks

#include <cv.h>
#include <highgui.h>
#include "DisplayEditor.h"


THISCLASS::ComponentRILGlobalModeExpt(SwisTrackCore *stc):
    Component(stc, wxT("RILGlobalModeExpt")),
    mShopTasksFound(false),
    mAllSHMFound(false),
    mRobotDevicesFound(false),
    mLogFilesCreated(false),
    mShopTask(),
    mShopTasks(0),
		mSHM(),
		mTime(),
		mBgImage(0),
    mMaterialCount(0),
    mTaskUrgency(INIT_URGENCY),
    mDeltaUrgency(DLETA_URGENCY),
    mStartPoint(),
    mEndPoint(),
		mStepCount(0),
		mTaskBroadcast(),
		mTaskBroadcasts(),
		mTaskUrgencyWriter(),
    mTaskWorkerWriter(),
    mRobotDeviceStateWriter(),
		mDisplayOutput(wxT("Output"), wxT("After Global Mode Expt")) {

	// Data structure relations
	mCategory = &(mCore->mCategoryOutput);
	AddDataStructureRead(&(mCore->mDataStructureImageGray));
	AddDataStructureRead(&(mCore->mDataStructureRobotDevices));
	AddDataStructureWrite(&(mCore->mDataStructureShopTasks));
	AddDataStructureWrite(&(mCore->mDataStructureRobotDevices));
	AddDisplay(&mDisplayOutput);

	// Read the XML configuration file
	Initialize();
}

THISCLASS::~ComponentRILGlobalModeExpt() {
}

void THISCLASS::OnStart() {

	// Read the reloadable parameters
	OnReloadConfiguration();
    //CheckBgImage();

}

void THISCLASS::OnReloadConfiguration() {
    // Load the background image
    if(mCore->mDataStructureImageGray.mImage){
        mBgImage = mCore->mDataStructureImageGray.mImage;
    }

    // Check Shop Tasks
    if(!mShopTasksFound){
        //parse/set shop tasks parameters
//        mMaterialCount = 10; // TODO: parse
//        mTaskUrgency = 1.0;
//        mDeltaUrgency = 0.1;
        CreateShopTasks(MAXSHOPTASK, mMaterialCount,\
         mTaskUrgency, mDeltaUrgency);
    }

    // Check Robot devices
    if(!mRobotDevicesFound){
        CheckRobotDevices();
    }

    // Check Log files
    if(!mLogFilesCreated){
        InitLogFiles();
    }

}
// =================== STEP Actions =================

void THISCLASS::OnStep() {
  // increase counter
  mStepCount++;
  printf("\n============ Step count: [%ld] =========\n", mStepCount);

  // Check reloadable settings
  OnReloadConfiguration();
  //CheckBgImage();

  // Checkout robots' current status from STATE_SHM
  //!TODO: All states update may not be found in each step
  CheckoutStateMessages(RILROBOTLIST);

  // update shop-tasks' urgency worker count etc.
  UpdateShopTasks(MAXSHOPTASK, MAXROBOT);

  // Commit TaskBroadcasts to Broadcast_SHM
  CommitTaskBroadcastMessages(RILROBOTLIST);

  //Log data
  LogLiveTaskInfo();
  LogLiveRobotDeviceStates();

  // impose tasks as circles on image
  DrawShopTasks(TASKS_CENTERS);

	// Set the display
	DisplayEditor de(&mDisplayOutput);
	if (de.IsActive()) {
	    de.SetParticles(mCore->mDataStructureParticles.mParticles);
		de.SetMainImage(mBgImage);
	}

	 printf("============End Step =========\n");
}



void THISCLASS::OnStepCleanup() {
    //reset state
    //printf("Doing step clean up\n");

}

void THISCLASS::OnStop() {
    mStepCount = 0; // rset
    mShopTasksFound = false;
    mAllSHMFound = false;
    mRobotDevicesFound = false;
    mLogFilesCreated = false;
}

//==========  Specific methods implementations =============


// Create count x ShopTasks setting ids with learning instrinsics
void THISCLASS::CreateShopTasks(int count, int matcount,\
 float urgency, float deltaurgency){
    for(int i=0; i < count ; i++){
        mShopTask.SetDefaults(i, matcount, urgency, deltaurgency);
        mShopTasks.push_back(mShopTask) ;
        printf("Added %d shop task\n", i);
    }

    // set mCore's mShopTasks
    //mCore->mDataStructureShopTasks.mShopTasks = &mShopTasks;
    MC_DS_SHOPTASKS = &mShopTasks;
    DataStructureShopTasks::tShopTaskVector *p = MC_DS_SHOPTASKS;
    DataStructureShopTasks::tShopTaskVector::iterator it = p->begin();
    //test print
    while(it != p->end()){
        printf("From %d shop-tasks: task %d: has unit %d \n",\
         (int)p->size(), it->mID, it->mMaterialCount);
        it++;
    }

    // setup locations
    //SetupTaskLocations(MAXSHOPTASK, TASKS_START, TASKS_END);
    SetupTaskCenters(MAXSHOPTASK, TASKS_CENTERS);

    //Create coresponding TaskBroadcast
    for(unsigned int i = 0; i < mShopTasks.size(); i++){
        int id = mShopTasks.at(i).mID;
        CvPoint2D32f center = mShopTasks.at(i).mCenter;
        double urgency = mShopTasks.at(i).mUrgency;
        AddTaskBroadcast(id, center, urgency);
    }

    // Set Flag
    mShopTasksFound = true;

 }

void THISCLASS::DrawShopTasks(float taskcenter[][XY])
{
  if (mBgImage && (!mShopTasks.empty())) {
    for (int i =0; i< MAXSHOPTASK; i++){
        CvPoint center = cvPoint(taskcenter[i][0], taskcenter[i][1]);
        cvCircle(mBgImage, center, TASK_RADIUS, CV_RGB(255, 0,0), 3);
    }
  }
}

void THISCLASS::AddTaskBroadcast(int& id, CvPoint2D32f& center, double& urgency)
 {
   mTaskBroadcast.id =  id;
   mTaskBroadcast.center.x = center.x;
   mTaskBroadcast.center.y = center.y;
   mTaskBroadcast.urgency = urgency;
   mTaskBroadcasts[id] = mTaskBroadcast;
 }


void THISCLASS::CheckRobotDevices()
{
    DataStructureRobotDevices::tRobotDeviceVector *p = MC_DS_ROBOTDEVICES;
	if (!p) {
	    printf("RobotDevices not created yet\n");
	    mRobotDevicesFound = false;
	} else {
        DataStructureRobotDevices::tRobotDeviceVector::iterator it = MC_DS_ROBOTDEVICES->begin();
	    //test print
	    while(it != p->end()){
	        printf("RobotDevice  %d: found \n", it->mID);
	        it++;
	    }
	    mRobotDevicesFound = true;
	}
}


//=================== SHM Stuff ==================
// Check All SHM for all robots
void THISCLASS::InitAllSHM(const char * robotlist[]){

    // clear previously created objects
    //mSHM.RemoveSHM(eTaskSHM); //asumed done by ComponentRobotDevMgr

    mSHM.InitTaskSHM(robotlist);

    // set flag
    mAllSHMFound = true;
}


//=============== Task related stuff =============
 void THISCLASS::SetupTaskLocations(int count, float taskstart[][XY], float taskend[][XY])
{

    DataStructureShopTasks::tShopTaskVector *p = MC_DS_SHOPTASKS;
    if (!p){
        printf("No Shop task found\n");
    } else {
        DataStructureShopTasks::tShopTaskVector::iterator it = p->begin();
        for (int i=0; i < count; i++){
            CvPoint pstart = cvPoint(taskstart[i][0], taskstart[i][1]);
            CvPoint pend = cvPoint(taskend[i][0], taskend[i][1]);
            it->SetLocation(pstart, pend);
            it++;
            printf("Updated task %d location\n", i);
        }
    }

}


 void THISCLASS::SetupTaskCenters(int count, float taskcenter[][XY])
{

    DataStructureShopTasks::tShopTaskVector *p = MC_DS_SHOPTASKS;
    if (!p){
        printf("No Shop task found\n");
    } else {
        DataStructureShopTasks::tShopTaskVector::iterator it = p->begin();
        for (int i=0; i < count; i++){
            CvPoint2D32f center = cvPoint2D32f(taskcenter[i][0], taskcenter[i][1]);
            it->SetCenter(center);
            it++;
            printf("Updated task %d location\n", i);
        }
    }

}

// SHM stuffs
void THISCLASS::CheckoutStateMessages(const char * robotlist[])
{
  int update = 0, noupdate = 0;
  StateMessageType msg;
  DataStructureRobotDevices::tRobotDeviceVector *p = MC_DS_ROBOTDEVICES;
  for(int i = 0; i < MAXROBOT; i++){
      msg = mSHM.CheckoutStateMessage(robotlist[i]);
      // test if the checkedout state is recent
      if (msg.step != p->at(i).mStateStep) { // update happened
         p->at(i).mState = msg.state;
         p->at(i).mStateStep = msg.step;
         update++;
      } else { // no update done, device lost or reset
         p->at(i).mState = RobotDevice::NOTSET;
         noupdate++;
      }

  }
  printf("CheckoutStateMessages(): got update: %d, not found: %d\n", update, noupdate);
}



void THISCLASS::UpdateShopTasks(int taskcount, int robotcount)
{
  int workercount = 0, task = -1;
  bool working = false;
  DataStructureRobotDevices::tRobotDeviceVector *p = MC_DS_ROBOTDEVICES;
  DataStructureShopTasks::tShopTaskVector* q = MC_DS_SHOPTASKS;
  if (p && q){
    for(int i=0; i< taskcount; i++){
	    for(int j = 0; j < robotcount; j++){
	      working = (p->at(j).mState > 100  ? true : false );
	      if(!working) continue;
	      task = (p->at(j).mState / 100) - 1; //decode task id
        workercount += (task == i ? 1 : 0 );
	    }
	    // update this shoptask
      q->at(i).StepUpdate(mStepCount, workercount);
      //printf("   UpdateShopTasks: task %d workers: %d  \n", i, workercount);
    }
  }
}


// Commit TaskBroadcasts to BROADCAST_SHM
void THISCLASS::CommitTaskBroadcastMessages(const char * robotlist[])
{
  TaskBroadcastMessageType msg;
  // update/sync. task broadcast structure
  for(unsigned int i=0; i< MAXSHOPTASK; i++){
    mTaskBroadcasts[i].urgency = mShopTasks.at(i).mUrgency;
    msg.tasks[i] = mTaskBroadcasts[i];
  }

  msg.step = mStepCount;

  // commit to SHM
  for(int i = 0; i < MAXROBOT; i++){
      mSHM.CommitTaskBroadcastMessage(robotlist[i], msg);
  }
}




void THISCLASS::InitLogFiles()
{
  /* Create log file to save tasks' urgency values*/
  char s1[DATA_ITEM_LEN];
  std::string objtype = "Task";
  LiveGraphDataWriter::DataContextType ctx;
  ctx.name = "Urgency";
  ctx.sep = ";";
  ctx.desc = "Task Urgency (Phi) in global broadcast mode";
  ctx.label = "TimeStamp;StepCounter";
  // preapre data label
  for(unsigned  int i=0; i< mShopTasks.size(); i++){
    sprintf(s1, ";Task%d", mShopTasks.at(i).mID);
    ctx.label += s1;
  }

  mTaskUrgencyWriter.InitDataFile(objtype, &ctx);

  /* Create log file to save all tasks' worker count*/
  ctx.name = "WorkerCount";
  ctx.desc = "Task Workers in global broadcast mode";

  mTaskWorkerWriter.InitDataFile(objtype, &ctx);;

  /* Create log file to save all tasks' worker count*/
  objtype = "RobotActivity";
  ctx.name = "States";
  ctx.desc = "Robot Conncetivity and task status in global broadcast experiment";
  ctx.label = "TimeStamp;StepCounter";
  // preapre data label
  DataStructureRobotDevices::tRobotDeviceVector *p = MC_DS_ROBOTDEVICES;
  DataStructureRobotDevices::tRobotDeviceVector::iterator it = MC_DS_ROBOTDEVICES->begin();
  //test print
  while(it != p->end()){
    sprintf(s1, ";Robot%d", it->mID);
    ctx.label += s1;
    it++;
  }

  mRobotDeviceStateWriter.InitDataFile(objtype, &ctx);;

  // set flag
  mLogFilesCreated = true;

}


void THISCLASS::LogLiveTaskInfo()
{
 /* Save all tasks' urgency values to log file*/
  //char *sep;
  static char  s1[DATA_ITEM_LEN];
  static char step[DATA_ITEM_LEN];
  std::string time, data, sep;
  sep = DATA_SEP;
  double u;
  wxString s2;
  time = mTaskUrgencyWriter.GetTimeStamp();
  data.append(time);

  data.append(DATA_SEP);
  sprintf(step, "%ld", mStepCount); // Use this component's step count
  data.append(step);

  DataStructureShopTasks::tShopTaskVector *p = MC_DS_SHOPTASKS;
  DataStructureShopTasks::tShopTaskVector::iterator it = p->begin();
  //append urgency
  while(it != p->end()){
    u = Utility::Trunc(it->mUrgency, 3);
    s2 = wxString::Format(wxT("%f"), it->mUrgency);
    data.append(sep);
    data.append(s2);
    it++;
  }

  mTaskUrgencyWriter.AppendData(data);

  /* Save all tasks' worker count to a file*/
  mTaskWorkerWriter.GetTimeStamp(time);
  data = time;
  data.append(DATA_SEP);
  sprintf(step, "%ld", mStepCount); // Use this component's step count
  data.append(step);
  it = p->begin();
  while(it != p->end()){
      s2 = wxString::Format(wxT("%d"), it->mWorkers);
      data.append(sep);
      data.append(s2);
      it++;
  }

  mTaskWorkerWriter.AppendData(data);
}

void THISCLASS::LogLiveRobotDeviceStates()
{
  //char  *sep;
  char s1[DATA_ITEM_LEN];
  char step[DATA_ITEM_LEN];
  std::string time, sep, data;
  wxString s2;
  time = mTaskUrgencyWriter.GetTimeStamp();
  data.append(time);
  data.append(DATA_SEP);
  sprintf(step, "%ld", mStepCount); // Use this component's step count
  data.append(step);

  DataStructureRobotDevices::tRobotDeviceVector *p = MC_DS_ROBOTDEVICES;
  DataStructureRobotDevices::tRobotDeviceVector::iterator it = MC_DS_ROBOTDEVICES->begin();
  //test print
  while(it != p->end()){
    if(it->mState == RobotDevice::NOTSET){
      sprintf(s1, "; ");      // blank, no client there or no update happend
    } else if(it->mState == RobotDevice::UNAVAILABLE){
      sprintf(s1, ";%d", ( -1 * it->mID));
    } else {
      sprintf(s1, ";%d", ((int)it->mState + it->mID));
    }

    data.append(s1);
    it++;
  }
  mRobotDeviceStateWriter.AppendData(data);
}



