#ifndef Header_RobotTaskSelector
#define Header_RobotTaskSelector

#include "RobotDevice.h"
#include "DataStructureShopTasks.h"

#define NEXTSTART 0.001 // for range creation

//! A class that selects a task for a given RobotDevice from the given ShopTasks

class RobotTaskSelector {
  public:
     typedef std::vector<float> tTaskStimuliVector;
     typedef std::vector<float> tTaskProbabilityVector;

     tTaskStimuliVector mStimulus;
     tTaskProbabilityVector mProbabilities;

     struct Range {
         float start;
         float end;
     };

     typedef std::vector<Range> tTaskRangeVector;
     tTaskRangeVector mTaskRanges;

     int mRandNum; // generated rand()

     int mSelectedTaskID; // by matching random number with ranges
     bool mTaskSelected;

	//! Constructor.
	RobotTaskSelector(): mStimulus(), mProbabilities(), mTaskRanges(),\
	 mRandNum(0), mSelectedTaskID(-1), mTaskSelected(false) {
    }

    //! Destructor.
     ~RobotTaskSelector(){}


    //! Do all operations of AFM algorithm and return selected task's id
    int SelectTask(RobotDevice* robot,  DataStructureShopTasks::tShopTaskVector* v);


     //! Calculate stimulus
    void CalculateProbabilities(RobotDevice* robot,\
     DataStructureShopTasks::tShopTaskVector* taskvec);
    //! Calculate and set ranges
    void ConvertProbabilitiesIntoRange(double nextstart);
    //! Generate random number and set selected task
    void DoRandomSelection();

    // helper functions
    double CalculateStimuli( double dist, double deltadist, double learn,\
      double  urgency);
    double CalculateDist(CvPoint2D32f p1, CvPoint2D32f p2);


};

#endif
