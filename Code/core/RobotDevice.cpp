#include "RobotDevice.h"
#define THISCLASS RobotDevice

void THISCLASS::SetState(RobotDevice::eState state)
{
    mState = state;
}

THISCLASS::eState THISCLASS::GetState()
{
    return mState ;
}

void THISCLASS::SetTask(int task)
{
        mShopTask.mID = task;

}

int THISCLASS::GetTask()
{
    return mShopTask.mID;
}


void THISCLASS::SetDefaults(int id, double learn, double forget){
    mID = id;
    mLearnRate = learn;
    mForgetRate = forget;
}

void THISCLASS::InitTaskRecords(int taskcount){
  tTaskRecord taskrecord;
  for(int i=0; i < taskcount; i++ ){
    mTaskRecords.push_back(taskrecord);
  }
}

// master upadte fn
void THISCLASS::UpdateTaskRecords(int selectedTaskID){
  UpdateTaskTimesDone(selectedTaskID);
  UpdateTaskSensitization(selectedTaskID);
}


void THISCLASS::UpdateTaskTimesDone(int selectedTaskID){
  for(int i = 0; i < mTaskRecords.size(); i++){
    if(mTaskRecords.at(i).mID == selectedTaskID){
      ++mTaskRecords.at(i).mTimesDone;
    }
  }
}

void THISCLASS::UpdateTaskSensitization(int selectedTaskID){

  for(int i = 0; i < mTaskRecords.size(); i++){
    if(mTaskRecords.at(i).mID == selectedTaskID){
      mTaskRecords.at(i).mSensitization += mLearnRate;
    } else {
      mTaskRecords.at(i).mSensitization -= mForgetRate;
      if(mTaskRecords.at(i).mSensitization < 0)
        mTaskRecords.at(i).mSensitization = 0;
    }
  }
}


double THISCLASS::GetSensitization(int taskID){
  // look up into the task records by taskid
  return mTaskRecords.at(taskID).mSensitization;
}

void THISCLASS::PrintPose()
{
  printf("Robot %d pose(x,y,theta):  %.0f %.0f %.2f \n", mID, mPose.center.x,\
   mPose.center.y, mPose.orient);
}
