#include "ComponentRobotDeviceManager.h"
#define THISCLASS ComponentRobotDeviceManager

// shortcuts
#define MC_DS_ROBOTDEVICES mCore->mDataStructureRobotDevices.mRobotDevices

#include <cv.h>
#include <highgui.h>
#include "DisplayEditor.h"

THISCLASS::ComponentRobotDeviceManager(SwisTrackCore *stc):
    Component(stc, wxT("RobotDeviceManager")),
    mRobot(),
    mRobotDevices(0),
		mSHM(),
		mRobotDevicesFound(false),
    mAllSHMFound(false),
		mBgImage(0),
		mStepCount(),
		mDisplayOutput(wxT("Output"), wxT("After RobotDeviceManager worked")) {

	// Data structure relations
	mCategory = &(mCore->mCategoryOutput);
	AddDataStructureRead(&(mCore->mDataStructureImageGray));
	AddDataStructureRead(&(mCore->mDataStructureParticles));
	AddDataStructureWrite(&(mCore->mDataStructureRobotDevices));
	AddDisplay(&mDisplayOutput);

	// Read the XML configuration file
	Initialize();
}

THISCLASS::~ComponentRobotDeviceManager() {
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


    if(!CheckRobotDevices()){
         //parse/set robot devics parameters
        CreateRobotDevices(MAXROBOT, RILROBOTLIST, INIT_LEARN_RATE, INIT_FORGET_RATE);
    }

    //SHM Stuff
    if(!mAllSHMFound){
        InitAllSHM(RILROBOTLIST);
    }

}
// =================== STEP Actions =================

void THISCLASS::OnStep() {
    // increase counter
    mStepCount++;
    // Check Camera input
    OnReloadConfiguration();
    //CheckBgImage();
    //printf("Step count: %d \n", mStepCount);

    // state checkout
    UpdateRobotPoses();
    CommitPoseMessages(RILROBOTLIST);

	// Set the display
	DisplayEditor de(&mDisplayOutput);
	if (de.IsActive()) {
	    de.SetParticles(mCore->mDataStructureParticles.mParticles);
		de.SetMainImage(mBgImage);
	}
}

void THISCLASS::OnStepCleanup() {
    //reset state
    //printf("Doing step clean up\n");

}

void THISCLASS::OnStop() {
    //SHM cleanup
    CleanupAllSHM(SHM_COUNT, RILROBOTLIST);
    mStepCount = 0; // rset
}

// Task Specific implementations
// Check if all RobotDevices exists
bool THISCLASS::CheckRobotDevices(){
    // check objects of RobotDevice
DataStructureRobotDevices::tRobotDeviceVector *p = MC_DS_ROBOTDEVICES;
	if (!p) {
	    printf("RobotDevices not created yet\n");
	    mRobotDevicesFound = false;
		return false;
	} else {
        DataStructureRobotDevices::tRobotDeviceVector::iterator it = mRobotDevices.begin();
	    //test print
	    if(mStepCount < 2){
        while(it != p->end()){
            printf("RobotDevice  %d: found \n", it->mID);
            it++;
        }
	    }
	    mRobotDevicesFound = true;
	    return true;
	}
}

// Create count x RobotDevices setting ids with learning instrinsics
void THISCLASS::CreateRobotDevices(int count, const char* robotlist[],\
 float learn, float forget){
    for(int i=0; i < count ; i++){
        mRobot.SetDefaults(atoi(robotlist[i]), learn, forget);
        mRobot.InitTaskRecords(MAXSHOPTASK);
        mRobotDevices.push_back(mRobot) ;
        printf("Added %d robot\n", i);
    }

    // set mCore's mDSRobotDevices
    //mCore->mDataStructureRobotDevices.mRobotDevices = &mRobotDevices;
    MC_DS_ROBOTDEVICES = &mRobotDevices;
    DataStructureRobotDevices::tRobotDeviceVector *p = MC_DS_ROBOTDEVICES;
    DataStructureRobotDevices::tRobotDeviceVector::iterator it = p->begin();
    //test print
    while(it != p->end()){
        printf("From %d robots: Robot %d: can forget %.2f \n",\
         p->size(), it->mID, it->mForgetRate);
        it++;
    }
    // Set Flag
    mRobotDevicesFound = true;

 }

// Update pose from a Particle poses
void THISCLASS::UpdateRobotPoses(){
    printf(" ---RobotDeviceManager: Updating poses ---\n");
    DataStructureParticles::tParticleVector *p = mCore->mDataStructureParticles.mParticles;
    if (!p) {
      return;
    }
    //printf("  Some poses available... \n");
    DataStructureRobotDevices::tRobotDeviceVector *r = MC_DS_ROBOTDEVICES;
    if (!r) {
      return;
    }
    for(int i = 0; i < r->size(); i++){
      //printf("  Updating Robot %d \n", r->at(i).mID);
      DataStructureParticles::tParticleVector::iterator it = p->begin();
      while (it != p->end()){
        //printf("  Particle %d pose available \n", it->mID );
        if(it->mID == r->at(i).mID){ // ID compare
          //printf(" +++ Copied pose from particle %d to robot %d \n", it->mID, r->at(i).mID);
          r->at(i).mPose.center.x = it->mCenter.x;
          r->at(i).mPose.center.y = it->mCenter.y;
          r->at(i).mPose.orient = it->mOrientation;
          r->at(i).PrintPose();
          // update pose step
          r->at(i).mPoseStep = mStepCount;
        }
        it++;
      } //end while
    } // end for
}

//=================== SHM Stuff ==================S
// Create or open all SHM blocks for all robots
void THISCLASS::InitAllSHM(const char * robotlist[]){

    // clear previously created objects
    printf("Removing all SHM...\n");

    mSHM.RemoveSHM(ePoseSHM);
    mSHM.RemoveSHM(eStateSHM);
    mSHM.RemoveSHM(eTaskSHM);
    mSHM.RemoveSHM(eBroadcastSHM);

    // init
    mSHM.InitPoseSHM(robotlist);
    mSHM.InitStateSHM(robotlist);
    mSHM.InitTaskSHM(robotlist);
    mSHM.InitBroadcastSHM(robotlist);

    // set flag
    mAllSHMFound = true;
}

void THISCLASS::CleanupAllSHM(int shmcount, const char * robotlist[]){

    // clear previously created objects
    mSHM.CleanupSHM(ePoseSHM, robotlist);
    mSHM.CleanupSHM(eStateSHM, robotlist);
    mSHM.CleanupSHM(eTaskSHM, robotlist);
    mSHM.CleanupSHM(eBroadcastSHM, robotlist);

    // clear SHM
    mSHM.RemoveSHM(ePoseSHM);
    mSHM.RemoveSHM(eStateSHM);
    mSHM.RemoveSHM(eTaskSHM);
    mSHM.RemoveSHM(eBroadcastSHM);

    // set flag
    mAllSHMFound = false;
}

// Commit Pose info to POSE_SHM
void THISCLASS::CommitPoseMessages(const char * robotlist[]){
    PoseMessageType msg;
    printf(" ---RobotDeviceManager: Commiting poses ---\n");
    DataStructureRobotDevices::tRobotDeviceVector *r = MC_DS_ROBOTDEVICES;
    if (!r) {
      return;
    }

    for(int i = 0; i < r->size(); i++){
      msg.pose = r->at(i).mPose;
      msg.step = r->at(i).mPoseStep;
      mSHM.CommitPoseMessage(robotlist[i], msg);
    } // end for
}

