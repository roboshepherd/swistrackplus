#ifndef HEADER_ComponentInputCameraGigE
#define HEADER_ComponentInputCameraGigE
#define USE_CAMERA_PROSILICA_GIGE

#include "Component.h"

#if defined (USE_CAMERA_PYLON_GIGE)
#include <pylon/TlFactory.h>
#include <pylon/Result.h>
#include <pylon/gige/BaslerGigECamera.h>
#include <wx/thread.h>
#include <wx/event.h>
#include "Display.h"

//! An input component for GigE cameras.
class ComponentInputCameraGigE: public Component {

public:
	enum eTriggerMode {
		sTrigger_Timer = 0,			//!< Use the internal camera timer to start acquisition.
		sTrigger_InputLine1 = 1,	//!< Use input line 1 to start acquisition.
		sTrigger_InputLine2 = 2,	//!< Use input line 2 to start acquisition.
		sTrigger_InputLine3 = 3,	//!< Use input line 3 to start acquisition.
		sTrigger_InputLine4 = 4,	//!< Use input line 4 to start acquisition.
		sTrigger_Software = 5,		//!< Send a packet over the ethernet cable to start acquisition.
	};

	//! Constructor.
	ComponentInputCameraGigE(SwisTrackCore *stc);
	//! Destructor.
	~ComponentInputCameraGigE();

	// Overwritten Component methods
	void OnInitializeStatic();
	void OnTerminateStatic();
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();
	Component *Create() {
		return new ComponentInputCameraGigE(mCore);
	}

private:
	wxString mCameraFullName;							//!< (configuration) Full name of the camera (as returned by Pylon::DeviceInfo::GetFullName()).
	bool mColor;										//!< (configuration) Whether to acquire color or mono images.
	eTriggerMode mTriggerMode;							//!< (configuration) The trigger source.
	double mTriggerTimerFPS;							//!< (configuration) The FPS of the trigger timer.
	int mInputBufferSize;								//!< (configuration) The number of input buffer images.

	Pylon::CBaslerGigECamera *mCamera;					//!< Camera object.
	Pylon::CBaslerGigEStreamGrabber *mStreamGrabber;	//!< Stream grabber object.

	static const int mInputBufferSizeMax = 32;							//!< The maximum number of input buffer images.
	IplImage *mInputBufferImages[mInputBufferSizeMax];					//!< The input buffer images.
	Pylon::StreamBufferHandle mInputBufferHandles[mInputBufferSizeMax];	//!< The corresponding buffer handles.
	Pylon::GrabResult mCurrentResult;									//!< The current result.

	int mFrameNumber;									//!< The frame number since the component was started.
	IplImage *mOutputImage;								//!< The current output image (only used for color acquisition, for mono acquision one of the mInputBufferImages is used).

	//! The thread waiting for new images (in case of external trigger).
class Thread: public wxThread {
	public:
		ComponentInputCameraGigE *mComponent;		//!< The associated component.

		//! Constructor.
		Thread(ComponentInputCameraGigE *c): wxThread(), mComponent(c) {}
		//! Destructor.
		~Thread() {}

		// wxThread methods
		ExitCode Entry();
	};

	wxCriticalSection mThreadCriticalSection; 	//!< The critical section object to synchronize with the thread.

	//! Prepares the output image (recreates the image if necessary).
	inline void PrepareOutputImage(IplImage *inputimage) {
		if (mOutputImage) {
			if ((mOutputImage->width == inputimage->width) && (mOutputImage->height == inputimage->height) && (mOutputImage->depth == inputimage->depth)) {
				return;
			}
			cvReleaseImage(&mOutputImage);
		}
		mOutputImage = cvCreateImage(cvSize(inputimage->width, inputimage->height), inputimage->depth, 3);
	}
};
// ================= START Prosilica Camera stuff ===================
#elif defined (USE_CAMERA_PROSILICA_GIGE)
#include <wx/thread.h>
#include <wx/event.h>
#include "Display.h"
//Keep this exact order of next 3 includes below
#define _LINUX
#define _x64
#include <PvApi.h>
//#define PVDECL
#define CH_MONO 1	// Single channel for mono image
#define CH_COLOR 3	// 3 channels for color image

//! An input component for GigE cameras.
class ComponentInputCameraGigE: public Component {

public:
	enum eTriggerMode {
		sTrigger_Freerun = 0,		//!< Continuous Trigger
		sTrigger_SyncIn1 = 1,	//!< Use input line 1 to start acquisition.
		sTrigger_SyncIn2 = 2,	//!< Use input line 2 to start acquisition.
		sTrigger_SyncIn3 = 3,	//!< Use input line 3 to start acquisition.
		sTrigger_SyncIn4 = 4,	//!< Use input line 4 to start acquisition.
		sTrigger_FixedRate = 5,	//!< Fixed frame-rate generator
		sTrigger_Software = 6,	//!< Acquire when FrameStartSoftwarr command is run
	};
    enum eExposureMode {
        sExposure_Manual = 0,
        sExposure_Auto = 1,
        sExposure_AutoOnce = 2,
        sExposure_External = 3,
    };
	//! Constructor.
	ComponentInputCameraGigE(SwisTrackCore *stc);
	//! Destructor.
	~ComponentInputCameraGigE();

	// Overwritten Component methods
	void OnInitializeStatic();
	void OnTerminateStatic();
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();
	Component *Create() {
		return new ComponentInputCameraGigE(mCore);
	}

private:
	wxString mCameraFullName;		//!< (configuration) Full name of the camera (can bereturned by PvCameraInfo).
	bool mColor;				    //!< (configuration) Whether to acquire color or mono images.
	eTriggerMode mTriggerMode;		//!< (configuration) The trigger source to start frame.
	eExposureMode mExposureMode;    //!< (configuration) Camera exposure mode options
	tPvUint32 mExposureValue;             //!< (configuration) Set exposure value in manual mode
	tPvUint32 mAnalogGain;                //!< (configuration) Set the camera analog gain
	double mTriggerTimerFPS;		//!< (configuration) The FPS of the trigger timer. [UNUSED]
	int mInputBufferSize;			//!< (configuration) The number of input buffer images. [UNUSED]

	// camera structures
    unsigned long   mCameraUID; 		//! unique camera ID
    tPvHandle       mCameraHandle; 		//! handle for a camera
    tPvFrame        mCameraFrame;		//! frame description
    tPvCameraInfo	mCameraInfo;  		//!< Camera description
    unsigned long	mFrameSize;  		//!<  Frame Size
	tPvUint32   mWidth; 			//!<  Image width
	tPvUint32   mHeight; 			//!< Image height
    int mFrameNumber;			//!< The frame number since the component was started.
    tPvErr		mErrcode; 		//!< Prosilica tPvErr type error code
	IplImage   *mBayerImage; 		//!< captured raw (Bayer8) image
	IplImage   *mColorImage; 		//! converted image using CV_BayerGB2BGR
	IplImage   *mGrayImage; 		//! converted image using CV_BGRA2GRAY

	//static const int mInputBufferSizeMax = 32;		//!< The maximum number of input buffer images. [UNUSED]
	//IplImage *mInputBufferImages[mInputBufferSizeMax];	//!< The input buffer images. [UNUSED]

  	//! The thread waiting for new images (in case of external trigger).
  	class Thread: public wxThread {
	  public:
		ComponentInputCameraGigE *mComponent;	//!< The associated component.

		//! Constructor.
		Thread(ComponentInputCameraGigE *c): wxThread(), mComponent(c) {}
		//! Destructor.
		~Thread() {}

		// wxThread methods
		ExitCode Entry();
	};

	wxCriticalSection mThreadCriticalSection; 	//!< The critical section object to synchronize with the thread.

	//! Make some delay for camera driver initialization, if necessary
	inline void Sleep(unsigned int time)
	{
        	struct timespec t,r;
        	t.tv_sec    = time / 1000;
        	t.tv_nsec   = (time % 1000) * 1000000;

	        while(nanosleep(&t,&r)==-1)
        	t = r;
    	}

};
// ====================== END Prosilica Stuff =======================
#else // No GigE Support Available

class ComponentInputCameraGigE: public Component {

public:
	ComponentInputCameraGigE(SwisTrackCore *stc): Component(stc, wxT("InputCameraGigE")) {
		Initialize();
	}
	~ComponentInputCameraGigE() {}

	// Overwritten Component methods
	void OnStart() {
		AddError(wxT("GigE support was not compiled into this executable."));
	}
	void OnReloadConfiguration() {
		AddError(wxT("GigE support was not compiled into this executable."));
	}
	void OnStep() {
		AddError(wxT("GigE support was not compiled into this executable."));
	}
	void OnStepCleanup() {}
	void OnStop() {
		AddError(wxT("GigE support was not compiled into this executable."));
	}
	Component *Create() {
		return new ComponentInputCameraGigE(mCore);
	}
};

#endif // USE_CAMERA_PYLON_GIGE

#endif

