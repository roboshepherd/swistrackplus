#include "RobotTaskSelector.h"
#define THISCLASS RobotTaskSelector

#include <math.h>
#include <cv.h>

#include "Random.h"

#define DELTA_DISTANCE 0.001
//#define INIT_LEARNING 1.0

int THISCLASS::SelectTask(RobotDevice* robot,\
 DataStructureShopTasks::tShopTaskVector* taskvec)
 {
    CalculateProbabilities(robot, taskvec);

    ConvertProbabilitiesIntoRange(NEXTSTART);

    DoRandomSelection();

    return mSelectedTaskID;
 }



void THISCLASS::CalculateProbabilities(RobotDevice* robot,\
 DataStructureShopTasks::tShopTaskVector* taskvec)
 {
   // Find stimulus for each tasks: fn(distance, learn(ignore now), urgency)
   float stimuli;
   double dist;
   double deltadist = DELTA_DISTANCE;
   double learn; // search specific sensitization

   mStimulus.clear();

   // fill up stimulus
   DataStructureShopTasks::tShopTaskVector::iterator it = taskvec->begin();
   int i = 0;
   while(it != taskvec->end()){
    dist = CalculateDist(robot->mPose.center, it->mCenter);
    learn = robot->GetSensitization(i);
    stimuli = CalculateStimuli(learn, dist, deltadist, it->mUrgency);
    mStimulus.push_back(stimuli);
    it++;
    i++;
   }

   // sum up all stimulus
   float sum = 0;
   tTaskStimuliVector::iterator itr = mStimulus.begin();
   while(itr != mStimulus.end()) {
     sum += (*itr);
     itr++;
   }


   // fill the probability vector
  mProbabilities.clear();
  float prob = 0;
  itr = mStimulus.begin();
  while(itr != mStimulus.end()) {
   prob = (*itr)/sum;
   mProbabilities.push_back(prob);
   itr++;
  }

 }

void THISCLASS::ConvertProbabilitiesIntoRange(double nextstart)
{
   Range r;
   //set first item's range
   tTaskProbabilityVector::iterator itr = mProbabilities.begin();
   r.start = 0;
   r.end = (*itr);
   mTaskRanges.push_back(r);
   // next items but last one
   for(int i = 1; i < (mProbabilities.size() - 1); i++ ) {
     r.start = (r.end + nextstart);
     r.end = r.end + mProbabilities.at(i);
     mTaskRanges.push_back(r);
   }
   //set last item's range
   r.start = (r.end + nextstart);
   r.end = 1.0;
   mTaskRanges.push_back(r);
}


void THISCLASS::DoRandomSelection()
{
  Random rng;
  rng.Initialize();
  double randnum = rng.Uniform(); // within 0 and 1
  mRandNum = randnum * 100; // scale
  rng.Uninitialize();

  // find the task that have this randnum within it's range
    int sn, en;
  for ( int i = 0 ; i < mTaskRanges.size() && (!mTaskSelected); i++){
    sn =  mTaskRanges.at(i).start * 100;
    en = mTaskRanges.at(i).end * 100;
    if ( (mRandNum >= sn) && (mRandNum <= en)){
      mSelectedTaskID = i;
      mTaskSelected = true;
      printf("Task selected: %d \n", i);
    }
  }

}

double THISCLASS::CalculateStimuli(double learn, double dist, double deltadist,\
 double  urgency)
{
    // for now: use only disance and urgency statically
    return tanh(learn * urgency / ( dist + deltadist));
}

double THISCLASS::CalculateDist(CvPoint2D32f p1, CvPoint2D32f p2)
{
  float x1 = p1.x;
  float y1 = p1.y;
  float x2 = p2.x;
  float y2 = p2.y;
  return sqrt((x2-x1)*(x2-x1) + (y2-y1)*(y2-y1));
}
