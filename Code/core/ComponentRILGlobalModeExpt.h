#ifndef HEADER_ComponentRILGlobalModeExpt
#define HEADER_ComponentRILGlobalModeExpt

#include <cstdlib>
#include <cstdio>
#include <vector>
#include <map>
#include <cv.h>
#include <highgui.h>
#include <fstream>
#include <wx/string.h>

#include "Component.h"

#include "DataStructureShopTasks.h"
#include "ShopTask.h"
#include "SHMConfig.h"
#include "LiveGraphDataWriter.h"
#include "RILSetup.h"
#include "Utility.h"

#include "RILObjects.h"


#define ROBOT_COUNT 2 // for test expt

//! A component that
//! Allocate Tasks by AFM
class ComponentRILGlobalModeExpt: public Component {

public:
	//! Constructor.
	ComponentRILGlobalModeExpt(SwisTrackCore *stc);
	//! Destructor.
	~ComponentRILGlobalModeExpt();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();
	Component *Create() {
		return new ComponentRILGlobalModeExpt(mCore);
	}

	inline void CheckBgImage(){
    	if (! mBgImage) {
		AddError(wxT("No Bg image."));
		return;
        }
	}

	// Implementation methods
	//! Create count x ShopTasks setting ids with learning instrinsics
	void CreateShopTasks(int count, int matcount, float urgency, float deltaurgency);
	//! Create TaskBroadcast vector, the structure to broadcast, selected from shoptasks
  void AddTaskBroadcast(int& id, CvPoint2D32f& center, double& urgency);

  //! Check valid robot devices exist
  void CheckRobotDevices();
  //! Log Files check
  void InitLogFiles();
  //! SHM checks
  void InitAllSHM(const char * robotlist[]);

  //! Setup task locations
  void SetupTaskLocations(int taskcount, float taskstart[][XY], float taskend[][XY]);
  void SetupTaskCenters(int taskcount, float taskcenter[][XY]);

  //! which robot doing what task now? Save onto shop tasks
  void CheckoutStateMessages(const char * robotlist[]);
  //! Using Attractive Field Model
  void UpdateShopTasks(int taskcount, int robotcount); // update urgency etc.

  //! Commit Task info to BROADCAST_SHM to all robot objects
  void CommitTaskBroadcastMessages(const char * robotlist[]);

  //! Write TaskStatus for LiveGraph plotting
  // two files: 1 for task urgency, 2 for number of worker count
  void LogLiveTaskInfo();
  //! Write robot devices states
  void LogLiveRobotDeviceStates();

  //! Draw Tasks
  void DrawShopTasks(float taskcenter[][XY]);

private:
  //flags
  bool mShopTasksFound;
  bool mAllSHMFound;
  bool mRobotDevicesFound;
  bool mLogFilesCreated;

  //! Shop tasks
  ShopTask mShopTask;
  DataStructureShopTasks::tShopTaskVector mShopTasks; //!< given to mDataStructureShopTasks
  //! SHM configuration space
  SHMConfig mSHM;

  wxDateTime mTime;
  IplImage *mBgImage;	//!< The background image from D/S InputShopFloorImage.

  //Task parameters parsing
  int mMaterialCount;
  float mTaskUrgency;
  CvPoint mStartPoint;     //!< Location start point in one diagonal
  CvPoint mEndPoint;     //!< Location end point in same diagonal

  long mStepCount; //!< step counter of this component

  BroadcastBuffer::tTaskBroadcast mTaskBroadcast;
  //BroadcastBuffer::TaskBroadcastVector mTaskBroadcasts; //!< contents of broadcast
  BroadcastBuffer::tTaskBroadcast mTaskBroadcasts[MAXSHOPTASK];

  LiveGraphDataWriter mTaskUrgencyWriter;
  LiveGraphDataWriter mTaskWorkerWriter;
  LiveGraphDataWriter mRobotDeviceStateWriter;

  Display mDisplayOutput;		//!< The DisplayImage showing the output of this component.
  CvFont mFontMain;
};

#endif
