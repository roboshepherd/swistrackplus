#ifndef HEADER_RobotDevice
#define HEADER_RobotDevice
#include <wx/datetime.h>
#include <cv.h>

#include<map>
#include<vector>

#include "ShopTask.h"
#include "NavigationCommand.h"
#include "NavigationEvent.h"



//! A RobotDevice.
class RobotDevice {

public:
	int mID;				//!< ID of the Robot
	enum eState {
      /* Connectivity states */
	    NOTSET = -100,
	    /* state as (- id) e.g. UNAVAILABLE  = - id
        UNAVAILABLE state of Robot 5 = - 5*/
	    UNAVAILABLE = -50,
	    /* state as (+ id) e.g. AVAILABLE  = state + id
        AVAILABLE state of Robot 5 =  5*/
	    AVAILABLE = 0,
	    /* task states, set state as (this value+id) e.g. RW = 50 + id*/
	    RW = 50,
	    TASK = 99, /* seldom used */
	    TASK1 = 100,
	    TASK2 = 200,
	    TASK3 = 300,
	    TASK4 = 400,
	    TASK5 = 500,
	    TASK6 = 600,
	    TASK7 = 700,
	    TASK8 = 800,
	    TASK9 = 900,
	    TASK10 = 1000,
	};

	//char * displaytag[MAXTAG]; //shown on tracker image as ID[Pose|State|Task]

	eState mState;
  // POSE copied from Particle
  struct tPose {
        tPose(): center(), orient(0){}
        CvPoint2D32f center;		//!< Robot center [pixels].
        double orient;			//!< The orientation angle of the robot [rad].
  } mPose;

    //Instrinsic
	double mLearnRate;      //!< As per AFM basic learning rate
	double mForgetRate;     //!< As per AFM basic forgetting rate = learing rate/CONST

	wxDateTime mStartTime;     //!< initialization time

  ShopTask mShopTask; //!< recently allocated task by task allocation component


	long mStateStep; //!< Managed by E-puck player client
	long mPoseStep; //!< Managed by Pose tracker, SwisTrack CompRDM
	long mBroadcastStep; //! Managed by Task Broadcaster, SwisTrack CompRGE

  struct tTaskRecord { // task cvonfiguration and performance record of this robot
    tTaskRecord(): mID(-1), mSensitization(INIT_SENSITIZATION), mTimesDone(0), mDuration() {}
    int mID;
    double mSensitization;
    int mTimesDone; // to profile efficiency
    wxDateTime mDuration; // in this task, to profile efficiency
  };

  typedef std::vector<tTaskRecord> tTaskRecordVector; //
  tTaskRecordVector mTaskRecords; //!< learning parameters, k, of all tasks


	//! Constructor.
	RobotDevice(): mID(-1), mState(NOTSET), mPose(),\
    mLearnRate(INIT_LEARN_RATE), mForgetRate(INIT_FORGET_RATE),\
    mStartTime(), mShopTask(),\
    mStateStep(0), mPoseStep(0), mBroadcastStep(0), mTaskRecords(){
    }
	//! Destructor.
	~RobotDevice() {
	}

  //! Set default values
  void SetDefaults(int id, double learn, double forget);
  void InitTaskRecords(int taskcount);

  //! State Access
  void SetState(RobotDevice::eState state);
  eState GetState();
  //! Task access
  void SetTask(int task);
  int GetTask();

  //! TaskRecordVector access by selectedTaskID key
  void UpdateTaskRecords(int selectedTaskID);
  // subs
  void UpdateTaskTimesDone(int selectedTaskID);
  void UpdateTaskSensitization(int selectedTaskID);

  double GetSensitization(int taskID);

  void PrintPose();
};

#endif
