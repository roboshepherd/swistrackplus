#ifndef HEADER_ComponentRobotDeviceManager
#define HEADER_ComponentRobotDeviceManager

#include <cstdlib>
#include <vector>
#include <map>
#include <cv.h>
#include <highgui.h>
#include <fstream>

#include "Component.h"

#include "RobotDevice.h"
#include "SHMConfig.h" // also defines our RIL setup

#include "RILObjects.h"

//! A component that creates Robot devices and their Shared memory blocks
//! Allocate Tasks by AFM
//! Dump log into one file/robot
class ComponentRobotDeviceManager: public Component {

public:
	//! Constructor.
	ComponentRobotDeviceManager(SwisTrackCore *stc);
	//! Destructor.
	~ComponentRobotDeviceManager();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();
	Component *Create() {
		return new ComponentRobotDeviceManager(mCore);
	}

	inline void CheckBgImage(){
    	if (! mBgImage) {
		AddError(wxT("No Bg image."));
		return;
        }
	}


	// Task implementation methods
	//! Check if all RobotDevices exists
	bool CheckRobotDevices();
	//! Create count x RobotDevices setting ids with learning instrinsics
	void CreateRobotDevices(int count, const char* robotlist[], float learn, float forget);
	//! Update pose from a Particle poses
	void UpdateRobotPoses();

	//! Check / Create / open all SHM blocks for all robots
	//bool CheckAllSHM(const char * robotlist);
	void InitAllSHM(const char * robotlist[]);
	void CleanupAllSHM(int shmcount, const char * robotlist[]);
	//! Commit Pose info to POSE_SHM
	void CommitPoseMessages(const char * robotlist[]);

private:

    //! Robot Devices
    RobotDevice mRobot;
    DataStructureRobotDevices::tRobotDeviceVector mRobotDevices; //!< given to mDataStructureRobotDevices
    //! SHM configuration space
    SHMConfig mSHM;

    //! flags
    bool mRobotDevicesFound;
    bool mAllSHMFound;


    IplImage *mBgImage;	//!< The background image from D/S InputShopFloorImage.

    long mStepCount; // step counter

    Display mDisplayOutput;		//!< The DisplayImage showing the output of this component.
};

#endif

