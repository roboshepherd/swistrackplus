#ifndef HEADER_ComponentEpuckAccess
#define HEADER_ComponentEpuckAccess

#include <wx/string.h>
#include <cv.h>
#include "Component.h"


/* Serail access library: wxCTB installed in /usr/local/include
Add below lines in Makefile.inc
# Components that depend on the wxCTB library (activate the following lines to compile with these components)
CPPFLAGS_ADD       += -I/usr/local/include/wx
LDLIBS_ADD         += -L/usr/local/lib -lwxctb-0.13
*/
#include "ctb-0.13/getopt.h"
#include "ctb-0.13/iobase.h"
#include "ctb-0.13/serport.h"
#include "ctb-0.13/timer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ACCESS_TIMEOUT 10000000 // for e-puck data r/w keep as large
#define MAX_ROBOT 8

//! A component that access E-puck robot via bluetooth serial link
class ComponentEpuckAccess: public Component {

public:
	//! Constructor.
	ComponentEpuckAccess(SwisTrackCore *stc);
	//! Destructor.
	~ComponentEpuckAccess();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();
	Component *Create() {
		return new ComponentEpuckAccess(mCore);
	}

    // functions named with Epuck* access physical devices
    // EpuckN* refers to multiple physical devices
	wxIOBase* EpuckInit(char *devname);
	void EpuckExit(wxIOBase* dev);
	void EpuckDataExchange(wxIOBase* dev);
	void EpuckQueryPose(wxIOBase* dev);
	void EpuckResetPose(wxIOBase* dev, double x, double y, double theta);
	void EpuckSetTargetPose(wxIOBase* dev, double x, double y, double theta);

	void SetDeviceNames(); // hard-coded to devices
	void EpuckGetDevice(int i);
	void EpuckNGetDevices(int devcount);
	void EpuckNInitDevices(int devcount);
	void EpuckNCloseDevices(int devcount);
	void SetParticleID(int i);
	void SetNParticleID(int devcount);
	void EpuckQueryResetPose(int i);
	void EpuckNQueryResetPoses(int devcount);
	void EpuckSetTargetPose(int i, double x, double y, double th);
	void CalculateTargetSteps(int i);
	void EpuckNGoToTargetPoses(int devcount);
	void EpuckGoToTargetPose(int i);

	void GetCurrentPose(int i);



	inline void Sleep(unsigned int time)
	{
        	struct timespec t,r;
        	t.tv_sec    = time / 1000;
        	t.tv_nsec   = (time % 1000) * 1000000;

	        while(nanosleep(&t,&r)==-1)
        	t = r;
    }




private:
    char* mDevName[MAX_ROBOT]; //!< (Configuration) Rfcomm device name for an E-puck
    wxIOBase* mDevice[MAX_ROBOT]; //!< Epuck serial device
    int mParticleID[MAX_ROBOT];   //!< E-puck marker ID
    //wxChar mPoseQueryCode; //!< (Configuration) Send this char to get current E-puck pose
    //wxChar mPoseResetCode; //!< (Configuration) Send this char to reset current E-puck pose
    //wxChar mPoseUpdateCode; //!< (Configuration) Write: {(this Code),x,y,theta} to E-puck
    double mWCX[MAX_ROBOT]; //!< mWorldCenter.x
    double mWCY[MAX_ROBOT]; //!< mWorldCenter.x
    double mOrient[MAX_ROBOT]; //!< mOrientation

    double mTargetX[MAX_ROBOT]; //!< target pose x
    double mTargetY[MAX_ROBOT]; //!< target pose y
    double mTargetTheta[MAX_ROBOT]; //!< target pose theta
    bool mTargetPoseDone[MAX_ROBOT]; //!< Reached the target pose?
    bool mTargetPoseSet[MAX_ROBOT]; //!< Set the target pose?

    // For a simple navigation algorithm
    int mXStepCounter[MAX_ROBOT]; //!< how many steps to travel towards X
    int mXSign[MAX_ROBOT]; //!< target direction X sign
    int mYStepCounter[MAX_ROBOT]; //!< how many steps to travel towards Y
    int mYSign[MAX_ROBOT]; //!< target direction Y sign
    bool mStepCounted[MAX_ROBOT]; //!< Count check
    int mXMovedStep[MAX_ROBOT];
    int mYMovedStep[MAX_ROBOT];
    bool mThetaMoved[MAX_ROBOT];



	Display mDisplayOutput;		//!< The DisplayImage showing the output of this component.
};

#endif

