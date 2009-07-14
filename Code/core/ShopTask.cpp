#include "ShopTask.h"
#define THISCLASS ShopTask

void THISCLASS::SetDefaults(int i, int matcount, float urgency,\
 float deltaurgency)
{
    mID = i;
    mMaterialCount = matcount;
    mUrgency = urgency;
    mDeltaUrgency = deltaurgency;
}

void THISCLASS::SetLocation(CvPoint start, CvPoint end)
{
    mStartPoint = start;
    mEndPoint = end;
}

void THISCLASS::SetCenter(CvPoint2D32f center)
{
  mCenter = center;
}


void THISCLASS::StepUpdate(long stepnum, int workercount){

  //  update step info, used also in  statistical analysis purpose
  mStepCount= stepnum;
  mWorkers = workercount;

  // update urgency based on the worker count
  if(workercount == 0){
    mUrgency += mDeltaUrgency;
  } else {
    mUrgency -= mDeltaUrgency;
  }
  // keep it within 0 to 1

  mUrgency = (mUrgency < 0 ) ? 0 : mUrgency ;
  mUrgency = (mUrgency > 1.0 ) ? 1.0 : mUrgency ;

  // TEST later
  // else if (workerlimit > MAX_WORKER_LIMIT) {mUrgency -= mDeltaUrgency;}

}
