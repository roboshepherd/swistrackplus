#include "ComponentEpuckAccess.h"
#define THISCLASS ComponentEpuckAccess

#include "DisplayEditor.h"

#include <cmath>

#define PROX_X 10 // target clearance
#define PROX_Y 10
#define PROX_THETA 0.5

#define STEP_MOVE 33 // cm moved in each step
#define STEP_WAIT 1500 // after sending target pose, wait to complete movement
#define INITDEV_WAIT 11000 //RESET components fix this

#define ACTIVE_ROBOTS 1

THISCLASS::ComponentEpuckAccess(SwisTrackCore *stc):
		Component(stc, wxT("EpuckAccess")),
		//mDevName(), mDevice(), mParticleID(8),
		//mPoseQueryCode(wxT('V')),
		//mPoseResetCode(wxT('W')),
        //mPoseUpdateCode(wxT('X')),
        mWCX(), mWCY(), mOrient(),
        mTargetX(), mTargetY(), mTargetTheta(),
        mTargetPoseDone(), mTargetPoseSet(),
        mXStepCounter(), mXSign(),
        mYStepCounter(), mYSign(),
        mStepCounted(),
        mXMovedStep(), mYMovedStep(), mThetaMoved(),
		mDisplayOutput(wxT("Output"), wxT("After accessing e-pucks")) {

	// Data structure relations
	mCategory = &(mCore->mCategoryOutput); //FIXME: Create a new category
	AddDataStructureRead(&(mCore->mDataStructureParticles));

	// Read the XML configuration file
	Initialize();
}

THISCLASS::~ComponentEpuckAccess() {
}

void THISCLASS::OnStart() {
    printf("ComponentEpuckAccess: OnStart... \n");
    //parse e-puck device name
    //Create a device  mDevName[X] = "/dev/rfcommYYY"
    SetDeviceNames(); // all MAX_ROBOT, hard-coded names

    // Get the device handle
    EpuckNGetDevices(ACTIVE_ROBOTS);

    //parse/set particle ID
    //mParticleID = GetConfigurationInt(wxT("PartilceID"), -1);
    SetNParticleID(ACTIVE_ROBOTS);


	// load other parameters:
	OnReloadConfiguration();
	return;
}

void THISCLASS::OnReloadConfiguration() {
    printf("ComponentEpuckAccess: OnReloadConfiguration... \n");


}

void THISCLASS::OnStep() {
    printf("ComponentEpuckAccess: Begin OnStep... \n");
    // do one by one
    int founddev = 0;
    // Check e-puck device
    for(int i = 0; i < ACTIVE_ROBOTS; i++){
        if (!mDevice[i]) {
            printf("E-puck device: %d not found.\n", i);
            // Get the device handle
            EpuckGetDevice(i);
            SetParticleID(i);
        } else {
            founddev++;
            printf("==== [%d] Begin step tasks =====\n",i);
            EpuckQueryResetPose(i);
            // Check a particle pose exists
            DataStructureParticles::tParticleVector *p = mCore->mDataStructureParticles.mParticles;
            if (!p) {
                printf("No e-puck poses found.\n");
                return;
            }
            GetCurrentPose(i);
            EpuckSetTargetPose(i, 4450, 870, 0);
            EpuckGoToTargetPose(i);
            printf("==== [%d] End step tasks =====\n",i);
        }
    }
 // if found none then wait and return
    if(founddev == 0){
        //Sleep(INITDEV_WAIT);
        return;
    }

	// Set the display
//	DisplayEditor de(&mDisplayOutput);
//	if (de.IsActive()) {
//		de.SetMainImage(mCore->mDataStructureImageBinary.mImage);
//	}
    //printf("ComponentEpuckAccess: End OnStep... \n");
}

void THISCLASS::OnStepCleanup(){
    printf("ComponentEpuckAccess: OnStepCleanup... \n");


//    if(mDevice) {
//        EpuckExit(mDevice);
//    }
}

void THISCLASS::OnStop() {
	//cvReleaseImage(&mMaskImage);
	printf("ComponentEpuckAccess: OnStop... \n");
    EpuckNCloseDevices(ACTIVE_ROBOTS);
}

// internal calls
void THISCLASS::GetCurrentPose(int i)
{
    DataStructureParticles::tParticleVector *p = mCore->mDataStructureParticles.mParticles;
    DataStructureParticles::tParticleVector::iterator it = p->begin();
	while (it != p->end()) {
	    //printf("Read particle: %d -- looking for %d\n", it->mID, mParticleID );
        if ((it->mID) == mParticleID[i]) {
            mWCX[i]= (it->mWorldCenter.x);
            mWCY[i] = (it->mWorldCenter.y);
            mOrient[i] = (it->mOrientation);
            printf(">>> Particle %d current pose: %f, %f, %f\n",\
             mParticleID[i], mWCX[i], mWCY[i], mOrient[i]);
            // Calculate required steps to reach target
            CalculateTargetSteps(mParticleID[i]);
        } else {
            printf("target particle: %d pose not found... \n", mParticleID[i]);
        }
		it++;
    }
}



void THISCLASS::SetDeviceNames()
{
    mDevName[0] = "/dev/rfcomm46";
    mDevName[1] = "/dev/rfcomm50";
    mDevName[2] = "/dev/rfcomm53";
    mDevName[3] = "/dev/rfcomm59";
    mDevName[4] = "/dev/rfcomm60";
    mDevName[5] = "/dev/rfcomm65";
    mDevName[6] = "/dev/rfcomm71";
    mDevName[7] = "/dev/rfcomm102";
}

void THISCLASS::EpuckGetDevice(int i)
{
    mDevice[i] = EpuckInit(mDevName[i]);
}

void THISCLASS::EpuckNGetDevices(int devcount)
{
    for(int i=0; i< devcount; i++){
        mDevice[i] = EpuckInit(mDevName[i]);
    }
}
void THISCLASS::EpuckNInitDevices(int devcount) {
    for(int i = 0; i < devcount; i++){
        int found = 0;
        if (!mDevice[i]) {
            printf("E-puck device: %d not found.\n", i);
        } else { found++; }

        // Get the device handle
        EpuckGetDevice(i);
        SetParticleID(i);

        // if found none then wait and return
        if(found == 0){
            Sleep(INITDEV_WAIT);
            return;
        }
    }
}

void THISCLASS::EpuckNCloseDevices(int devcount)
{
    for(int i=0; i< devcount; i++){
        if(mDevice[i]) {
            EpuckExit(mDevice[i]);
        }
    }
}

void THISCLASS::SetParticleID(int i)
{
        mParticleID[i]  = i+1;
}

void THISCLASS::SetNParticleID(int devcount)
{
    for(int i=0; i < devcount; i++){
        mParticleID[i]  = i+1;
    }
}

void THISCLASS::EpuckQueryResetPose(int i)
{
        EpuckQueryPose(mDevice[i]);
        EpuckResetPose(mDevice[i], 0, 0, 0);
}


void THISCLASS::EpuckNQueryResetPoses(int devcount)
{
    for(int i=0; i < devcount; i++){
        EpuckQueryPose(mDevice[i]);
        EpuckResetPose(mDevice[i], 0, 0, 0);
    }
}

void THISCLASS::EpuckSetTargetPose(int i, double x, double y, double th)
{
    if(!mTargetPoseSet[i]) {
        mTargetX[i] = x;
        mTargetY[i] = y;
        mTargetTheta[i] = th;
        printf("[%d] Set a target pose: %f, %f, %f \n", i, mTargetX[i], mTargetY[i], mTargetTheta[i]);
        mTargetPoseSet[i] = true;
    }
}

void THISCLASS::CalculateTargetSteps(int i)
{
    if(!mStepCounted[i]){
        mXStepCounter[i] = abs(mWCX[i]-mTargetX[i])/(STEP_MOVE*10); //cm -> mm
        mXSign[i] = (mWCX[i] < mTargetX[i]) ? (1) : (-1);
        mYStepCounter[i] = abs(mWCY[i] - mTargetY[i])/(STEP_MOVE*10); //cm -> mm
        mYSign[i] = (mWCY[i] < mTargetY[i]) ? (1) : (-1);
        mStepCounted[i] = true;
        printf("[i] TargetSteps: %d, %d Sign: %d, %d\n",\
         i, mXStepCounter[i], mYStepCounter[i], mXSign[i], mYSign[i]);
    }
}

void THISCLASS::EpuckNGoToTargetPoses(int devcount)
{
    for (int i = 0; i< devcount; i++){
        EpuckGoToTargetPose(i);
    }
}

void THISCLASS::EpuckGoToTargetPose(int i){
    if(!mTargetPoseDone[i]) {
        if(!mThetaMoved[i]){
            EpuckResetPose(mDevice[i], 0, 0, 0);
            EpuckSetTargetPose(mDevice[i], 0, 0, mTargetTheta[i]);
            Sleep(STEP_WAIT);
            mThetaMoved[i] = true;
            printf(" [%d] ==> Theta Done\n", i);
        }
        if ((mXMovedStep[i] < mXStepCounter[i]) &&
                   (mYMovedStep[i] < mYStepCounter[i])) {
                EpuckResetPose(mDevice[i], 0, 0, 0);
                EpuckSetTargetPose(mDevice[i],\
                  (mXSign[i] * STEP_MOVE), (mYSign[i] * STEP_MOVE), 0);
                mXMovedStep[i]++;
                mYMovedStep[i]++;
                //Sleep(STEP_WAIT); // to complete e-puck's movement
        } else if (mXMovedStep[i] < mXStepCounter[i]) {
                EpuckResetPose(mDevice[i], 0, 0, 0);
                EpuckSetTargetPose(mDevice[i],(mXSign[i] * STEP_MOVE), 0, 0);
                mXMovedStep[i]++;
                //Sleep(STEP_WAIT); // to complete e-puck's movement
        } else if (mYMovedStep[i] < mYStepCounter[i]) {
                EpuckResetPose(mDevice[i], 0, 0, 0);
                EpuckSetTargetPose(mDevice[i], 0, (mYSign[i] * STEP_MOVE), 0);
                mYMovedStep[i]++;
                //Sleep(STEP_WAIT); // to complete e-puck's movement
        }

        printf(" [%d] ==> completed X count:%d/%d Y count:%d/%d !!!! \n",\
            i, mXMovedStep[i], mXStepCounter[i], mYMovedStep[i],mXStepCounter[i]);

    } else {
        mTargetPoseDone[i] = true;
        printf("[%d] Reached: target pose: completed X:%d Y:%d !!!! \n",\
            i, mXMovedStep[i], mYMovedStep[i]);
    }
}

wxIOBase* THISCLASS::EpuckInit(char *devname){
    wxIOBase* dev;
    // device is a serial port
    dev = new wxSerialPort();
    if(dev){
        printf("EpuckInit(): device created\n");
    }
    Sleep(1000);
    // try to open the given port
    if(dev->Open(devname) < 0) {
      printf("----- EpuckInit(): Cannot open %s -----\n",devname);
      delete dev;
      return NULL;
    }
    // set the baudrate
    ((wxSerialPort*)dev)->SetBaudRate(wxBAUD_115200);

    // ok, device is ready for communication
    return dev;
}

 void THISCLASS::EpuckExit(wxIOBase* dev)
 {
    dev->Close();
    delete dev;
 }

void THISCLASS::EpuckDataExchange(wxIOBase* dev)
{
    char* receivedString = NULL;
    size_t readedBytes = 0;
    dev->Writev("V\n",2,ACCESS_TIMEOUT);
    int state = dev->ReadUntilEOS(receivedString,
						    &readedBytes,
						    "\n",
						    ACCESS_TIMEOUT);
    if(readedBytes > 0) {
	   printf("EpuckDataExchange() Got: (%i): %s\n",readedBytes,receivedString);
    }
    delete receivedString;
}

void THISCLASS::EpuckQueryPose(wxIOBase* dev)
{
    char* receivedString = NULL;
    size_t readedBytes = 0;
    dev->Writev("V\n",2,ACCESS_TIMEOUT);
    int state = dev->ReadUntilEOS(receivedString,
						    &readedBytes,
						    "\n",
						    ACCESS_TIMEOUT);
    if(readedBytes > 0) {
	   printf("EpuckQueryPose() Got: (%i): %s\n",readedBytes,receivedString);
    }
    delete receivedString;
}

void THISCLASS::EpuckResetPose(wxIOBase* dev, double x, double y, double theta)
{
    char* receivedString = NULL;
    size_t readedBytes = 0;
    char buf[31];
    sprintf(buf,"W,%6.2f,%6.2f,%1.2f\n", x, y,theta);
    printf("Reset pose:%s\n", buf);
    dev->Writev(buf,sizeof(buf),ACCESS_TIMEOUT);
    int state = dev->ReadUntilEOS(receivedString,
						    &readedBytes,
						    "\n",
						    ACCESS_TIMEOUT);
    //printf("Got state: %d\n", state);
    if(readedBytes > 0) {
	   printf("EpuckResetPose() Got: (%i): %s\n",readedBytes,receivedString);
    }
    delete receivedString;
}

void THISCLASS::EpuckSetTargetPose(wxIOBase* dev, double x, double y, double theta)
{
    char* receivedString = NULL;
    size_t readedBytes = 0;
    char buf[31];
    sprintf(buf,"X,%6.2f,%6.2f,%6.2f\n", x, y,theta);
    printf("Set target pose:%s\n", buf);
    dev->Writev(buf,sizeof(buf),ACCESS_TIMEOUT);
    int state = dev->ReadUntilEOS(receivedString,
						    &readedBytes,
						    "\n",
						    ACCESS_TIMEOUT);
    //printf("Got state: %d\n", state);
    if(readedBytes > 0) {
	   printf("EpuckSetTargetPose() Got: (%i): %s\n",readedBytes,receivedString);
    }
    delete receivedString;
}
