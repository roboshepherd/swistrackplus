#ifndef HEADER_ShopTask
#define HEADER_ShopTask
#include <cv.h>

#include<map>
#include<vector>

#include "RILSetup.h"



//#define MAX_WORKER_LIMIT 3

//! A ShopTask.
class ShopTask {

public:
	int mID;				//!< ID of the Shop task
	int mMaterialCount;         //! Total material units available

	double mUrgency;        //!< Task urgency[0~1], emergency cases it would be 1
	//double mDeltaUrgency;   //!< Change of task urgency in each step

  CvPoint2D32f mCenter;     //!< Location end point in same diagonal
  CvPoint mStartPoint;     //!< Location start point in one diagonal
  CvPoint mEndPoint;     //!< Location end point in same diagonal

  int mWorkers;
  long mStepCount;

	//! Constructor.
	ShopTask(): mID(-1), mMaterialCount(0),mUrgency(INIT_URGENCY),\
    mCenter(), mStartPoint(), mEndPoint(), mWorkers(), mStepCount(0) {}
	//! Destructor.
  ~ShopTask(){}

	// methods
	void SetDefaults(int id, int matcount, float urgency,\
        float deltaurgency);
	void SetLocation(CvPoint start, CvPoint end);
	void SetCenter(CvPoint2D32f center);

  void StepUpdate(long stepnum, int workercount);


};

#endif
