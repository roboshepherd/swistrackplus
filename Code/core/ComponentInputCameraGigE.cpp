#include "ComponentInputCameraGigE.h"
#define THISCLASS ComponentInputCameraGigE

#if defined (USE_CAMERA_PYLON_GIGE)
#include "DisplayEditor.h"
#include "ImageConversion.h"

THISCLASS::ComponentInputCameraGigE(SwisTrackCore *stc):
		Component(stc, wxT("InputCameraGigE")),
		mCamera(0), mStreamGrabber(0), mCurrentResult(),
		mFrameNumber(0), mOutputImage(0) {

	// Data structure relations
	mCategory = &(mCore->mCategoryInput);
	AddDataStructureWrite(&(mCore->mDataStructureInput));

	// Trigger
	mTrigger = new ComponentTrigger(this);

	// Read the XML configuration file
	Initialize();
}

THISCLASS::~ComponentInputCameraGigE() {
	// Free the output image
	if (mOutputImage) {
		cvReleaseImage(&mOutputImage);
	}

	delete mTrigger;
}

void THISCLASS::OnInitializeStatic() {
	Pylon::PylonInitialize();
}

void THISCLASS::OnTerminateStatic() {
	Pylon::PylonTerminate();
}

void THISCLASS::OnStart() {
	mCameraFullName = GetConfigurationString(wxT("CameraFullName"), wxT(""));
	mColor = GetConfigurationBool(wxT("Color"), true);
	mTriggerMode = (eTriggerMode)GetConfigurationInt(wxT("TriggerMode"), 0);
	mTriggerTimerFPS = GetConfigurationInt(wxT("TriggerTimerFPS"), 10);
	mInputBufferSize = GetConfigurationInt(wxT("InputBufferSize"), 8);
	mFrameNumber = 0;

	// Check the maximum amount of buffers
	if (mInputBufferSize < 1) {
		mInputBufferSize = 1;
		AddWarning(wxString::Format(wxT("Using %d input buffer."), mInputBufferSize));
	}
	if (mInputBufferSize > mInputBufferSizeMax) {
		mInputBufferSize = mInputBufferSizeMax;
		AddWarning(wxString::Format(wxT("Using %d input buffers."), mInputBufferSize));
	}

	// Get the transport layer
	Pylon::CTlFactory& tlfactory = Pylon::CTlFactory::GetInstance();
	try {
		if (mCameraFullName.Len() == 0) {
			Pylon::DeviceInfoList_t devices;
			if (tlfactory.EnumerateDevices(devices) == 0) {
				AddError(wxT("No GigE cameras found!"));
				return;
			}
			mCore->mEventRecorder->Add(SwisTrackCoreEventRecorder::sType_StepLapTime, this);
			mCamera = new Pylon::CBaslerGigECamera(tlfactory.CreateDevice(devices[0]));
			mCore->mEventRecorder->Add(SwisTrackCoreEventRecorder::sType_StepLapTime, this);
		} else {
			// Create a camera object and cast to the camera class
			GenICam::gcstring camera_full_name = GenICam::gcstring(mCameraFullName.mb_str());
			mCamera = new Pylon::CBaslerGigECamera(tlfactory.CreateDevice(camera_full_name));
			if (! mCamera) {
				AddError(wxT("Camera not found!"));
				return;
			}
		}

		// Open the camera object
		mCamera->Open();
	} catch (GenICam::GenericException &e) {
		printf("%s\n", e.GetDescription());
		AddError(wxT("Error while opening the camera."));
		mCamera = 0;
		return;
	}

	// Pixel format: color or mono
	if (mColor) {
		mCamera->PixelFormat.SetValue(Basler_GigECameraParams::PixelFormat_YUV422Packed);
	} else {
		mCamera->PixelFormat.SetValue(Basler_GigECameraParams::PixelFormat_Mono8);
	}

	// Area of interest
	int cameraw = mCamera->Width.GetMax();
	int camerah = mCamera->Height.GetMax();
	int aoix = GetConfigurationInt(wxT("AOIOffset.x"), 0);
	int aoiy = GetConfigurationInt(wxT("AOIOffset.y"), 0);
	int aoiw = GetConfigurationInt(wxT("AOISize.x"), 64);
	int aoih = GetConfigurationInt(wxT("AOISize.y"), 64);
	if (aoix > cameraw - 1) {
		aoix = cameraw - 1;
	}
	if (aoiy > camerah - 1) {
		aoiy = camerah - 1;
	}
	if (aoiw > cameraw - aoix) {
		aoiw = cameraw - aoix;
	}
	if (aoih > camerah - aoiy) {
		aoih = camerah - aoiy;
	}
	aoiw = aoiw ^ (aoiw & 0x3);	// The image width must currently be a multiple of 4, for alignment reasons.
	aoih = aoih ^ (aoih & 0x1);	// The image height must currently be a multiple of 2.
	if (aoiw < 4) {
		aoiw = 4;
	}
	if (aoih < 2) {
		aoih = 2;
	}
	mCamera->OffsetX.SetValue(aoix);
	mCamera->OffsetY.SetValue(aoiy);
	mCamera->Width.SetValue(aoiw);
	mCamera->Height.SetValue(aoih);

	// Continuous mode
	mCamera->AcquisitionMode.SetValue(Basler_GigECameraParams::AcquisitionMode_Continuous);

	// Trigger
	if (mTriggerMode == sTrigger_Timer) {
		mCamera->TriggerMode.SetValue(Basler_GigECameraParams::TriggerMode_Off);
		mCamera->AcquisitionFrameRateEnable.SetValue(true);
		mCamera->AcquisitionFrameRateAbs.SetValue(mTriggerTimerFPS);
	} else {
		mCamera->TriggerMode.SetValue(Basler_GigECameraParams::TriggerMode_On);
		mCamera->AcquisitionFrameRateEnable.SetValue(false);
		mCamera->TriggerSelector.SetValue(Basler_GigECameraParams::TriggerSelector_AcquisitionStart);
		mCamera->TriggerActivation.SetValue(Basler_GigECameraParams::TriggerActivation_RisingEdge);
		if (mTriggerMode == sTrigger_InputLine1) {
			mCamera->TriggerSource.SetValue(Basler_GigECameraParams::TriggerSource_Line1);
		} else if (mTriggerMode == sTrigger_InputLine2) {
			mCamera->TriggerSource.SetValue(Basler_GigECameraParams::TriggerSource_Line2);
		} else if (mTriggerMode == sTrigger_InputLine3) {
			mCamera->TriggerSource.SetValue(Basler_GigECameraParams::TriggerSource_Line3);
		} else if (mTriggerMode == sTrigger_InputLine4) {
			mCamera->TriggerSource.SetValue(Basler_GigECameraParams::TriggerSource_Line4);
		} else {
			mTriggerMode = sTrigger_Software;
			mCamera->TriggerSource.SetValue(Basler_GigECameraParams::TriggerSource_Software);
		}
	}

	// Configure reloadable values
	OnReloadConfiguration();

	// Get and open a stream grabber
	mCamera->GetStreamGrabber(0);
	mStreamGrabber = new Pylon::CBaslerGigECamera::StreamGrabber_t(mCamera->GetStreamGrabber(0));
	mStreamGrabber->Open();

	// Parameterize the stream grabber
	const int buffersize = mCamera->PayloadSize.GetValue();
	mStreamGrabber->MaxBufferSize.SetValue(buffersize);
	mStreamGrabber->MaxNumBuffer.SetValue(mInputBufferSize);
	mStreamGrabber->PrepareGrab();

	// Allocate and register image buffers, put them into the grabber's input queue
	try {
		int channels = (mColor ? 2 : 1);
		for (int i = 0; i < mInputBufferSize; ++i) {
			mInputBufferImages[i] = cvCreateImage(cvSize(aoiw, aoih), 8, channels);
			mInputBufferHandles[i] = mStreamGrabber->RegisterBuffer(mInputBufferImages[i]->imageData, mInputBufferImages[i]->imageSize);
			mStreamGrabber->QueueBuffer(mInputBufferHandles[i], mInputBufferImages[i]);
		}
	} catch (GenICam::GenericException &e) {
		printf("%s\n", e.GetDescription());
		AddError(wxT("Error while registering image buffers."));
		mStreamGrabber->Close();
		mStreamGrabber = 0;
		mCamera->Close();
		mCamera = 0;
		return;
	}

	// Start image acquisition
	mCamera->AcquisitionStart.Execute();

	// Prepare the trigger for the next frame
	if (mTriggerMode == sTrigger_Software) {
		// When using the software trigger, we are immediately ready to read the next image
		mTrigger->SetReady();
	} else {
		// Otherwise, restart a thread waiting for the next image
		ComponentInputCameraGigE::Thread *ct = new ComponentInputCameraGigE::Thread(this);
		ct->Create();
		ct->Run();
	}
}

void THISCLASS::OnReloadConfiguration() {
	if (! mCamera) {
		return;
	}

	try {
		// Configure exposure time and mode
		int exposuretime = GetConfigurationInt(wxT("ExposureTime"), 100);
		mCamera->ExposureMode.SetValue(Basler_GigECameraParams::ExposureMode_Timed);
		mCamera->ExposureTimeRaw.SetValue(exposuretime);

		// Configure analog gain
		int analoggain = GetConfigurationInt(wxT("AnalogGain"), 500);
		mCamera->GainRaw.SetValue(analoggain);
	} catch (GenICam::GenericException &e) {
		printf("%s\n", e.GetDescription());
		AddError(wxT("Error while setting camera parameters."));
		return;
	}
}

void THISCLASS::OnStep() {
	if (! mCamera) {
		return;
	}

	// Get the image
	if (mTriggerMode == sTrigger_Software) {
		// Send the software trigger
		mCamera->TriggerSoftware.Execute();
	} else {
		// Wait for the thread to terminate (this should return immediately, as the thread should have terminated already)
		wxCriticalSectionLocker csl(mThreadCriticalSection);
	}

	// Wait for the grabbed image with a timeout of 3 seconds
	if (! mStreamGrabber->GetWaitObject().Wait(3000)) {
		AddError(wxT("Failed to retrieve an image: the camera did not send any image."));
		return;
	}

	// Get an item from the grabber's output queue
	bool res = mStreamGrabber->RetrieveResult(mCurrentResult);
	if ((! res) || (! mCurrentResult.Succeeded())) {
		AddWarning(wxString::Format(wxT("Failed to retrieve an item from the output queue: %s"), mCurrentResult.GetErrorDescription().c_str()));
	}

	// This is the acquired image
	IplImage *outputimage = (IplImage*)(mCurrentResult.Context());

	// If we are acquireing a color image, we need to transform it from YUV422 to BGR, otherwise we use the raw image
	if (mColor) {
		PrepareOutputImage(outputimage);
		ImageConversion::CvtYUV422ToBGR(outputimage, mOutputImage);
		mCore->mDataStructureInput.mImage = mOutputImage;
	} else {
		mCore->mDataStructureInput.mImage = outputimage;
	}

	// Set the frame number
	mFrameNumber++;
	mCore->mDataStructureInput.mFrameNumber = mFrameNumber;
}

void THISCLASS::OnStepCleanup() {
	if (! mCamera) {
		return;
	}

	// Requeue the used image
	try {
		mStreamGrabber->QueueBuffer(mCurrentResult.Handle(), mCurrentResult.Context());
	} catch (GenICam::GenericException &e) {
		AddError(wxString::Format(wxT("Failed to requeue buffer: %s"), e.GetDescription()));
	}

	// Prepare the trigger for the next frame
	if (mTriggerMode == sTrigger_Software) {
		// When using the software trigger, we are immediately ready to read the next image
		mTrigger->SetReady();
	} else {
		// Otherwise, restart a thread waiting for the next image
		ComponentInputCameraGigE::Thread *ct = new ComponentInputCameraGigE::Thread(this);
		ct->Create();
		ct->Run();
	}
}

void THISCLASS::OnStop() {
	if (! mCamera) {
		return;
	}

	// The camera is in continuous mode, stop image acquisition
	mCamera->AcquisitionStop.Execute();

	// Flush the input queue, grabbing may have failed
	mStreamGrabber->CancelGrab();

	// Wait for the thread to quit
	wxCriticalSectionLocker csl(mThreadCriticalSection);

	// Consume all items from the output queue
	Pylon::GrabResult result;
	while (mStreamGrabber->GetWaitObject().Wait(0)) {
		mStreamGrabber->RetrieveResult(result);
	}

	// Deregister and free buffers
	for (int i = 0; i < mInputBufferSize; ++i) {
		mStreamGrabber->DeregisterBuffer(mInputBufferHandles[i]);
		cvReleaseImage(&mInputBufferImages[i]);
	}

	// Clean up
	mStreamGrabber->FinishGrab();
	mStreamGrabber->Close();
	mStreamGrabber = 0;
	mCamera->Close();
	Pylon::CTlFactory& tlfactory = Pylon::CTlFactory::GetInstance();
	tlfactory.DestroyDevice(mCamera->GetDevice());
	mCamera = 0;
}

wxThread::ExitCode THISCLASS::Thread::Entry() {
	wxCriticalSectionLocker csl(mComponent->mThreadCriticalSection);

	// Wait for the grabbed image
	while (! mComponent->mStreamGrabber->GetWaitObject().Wait(3000)) {
		// This is intentionally left empty
	}

	mComponent->mTrigger->SetReady();
	return 0;
}

#endif // USE_CAMERA_PYLON_GIGE

#if defined (USE_CAMERA_PROSILICA_GIGE)
#include "cv.h"
#include "cvaux.h"
#include "highgui.h"

#define MIN_IMG_W 800
#define MIN_IMG_H 600
#define MAX_IMG_W 4872
#define MAX_IMG_H 3248

THISCLASS::ComponentInputCameraGigE(SwisTrackCore *stc):
		Component(stc, wxT("InputCameraGigE")),mCameraUID(0),mCameraHandle(),mCameraFrame(),
		mCameraInfo(), mFrameSize(0), mWidth(0), mHeight(0),
		mFrameNumber(0), mErrcode(),
		mBayerImage(NULL),mColorImage(NULL),mGrayImage(NULL)
{

	// Data structure relations
	mCategory = &(mCore->mCategoryInput);
	AddDataStructureWrite(&(mCore->mDataStructureInput));

	// Trigger
	mTrigger = new ComponentTrigger(this);

	// Read the XML configuration file
	Initialize();
}

THISCLASS::~ComponentInputCameraGigE() {

  //delete trigger;
	delete mTrigger;
}

void THISCLASS::OnInitializeStatic() {
	PvInitialize();
}

void THISCLASS::OnTerminateStatic() {
	PvUnInitialize();
}

void THISCLASS::OnStart() {
	mCameraFullName = GetConfigurationString(wxT("CameraFullName"), wxT(""));
	mColor = GetConfigurationBool(wxT("Color"), false);
	mTriggerMode = (eTriggerMode)GetConfigurationInt(wxT("TriggerMode"), 0);
	mTriggerTimerFPS = GetConfigurationInt(wxT("TriggerTimerFPS"), 3); // [UNUSED]

	// Check camera initialization
	unsigned long numCams = 0;
  	numCams = PvCameraList(&mCameraInfo, 1, NULL);
    if (numCams == 1)
    {
      // Get the camera ID
      mCameraUID = mCameraInfo.UniqueId;
      printf("Found Camera ID \t\t= %d\n", mCameraUID);
      mCore->mEventRecorder->Add(SwisTrackCoreEventRecorder::sType_StepLapTime, this);
      } else {
		if (numCams < 0) {
		AddError(wxT("No GigE cameras found!"));
		return;
		}
     }

    // Open a  camera
	if(!PvCameraOpen(mCameraUID, ePvAccessMaster, &(mCameraHandle)))
    {
        //Reset camera config file to Factory or 1, 2, 3 etc.
        PvAttrEnumSet(mCameraHandle,"CofigFileIndex","Factory");
        mErrcode = PvCommandRun(mCameraHandle, "ConfigFileLoad");
        if (mErrcode != ePvErrSuccess)
        printf("Errcode: %d Could not load camera config: \n", mErrcode);

        // Get the image frame size
        mErrcode = PvAttrUint32Get(mCameraHandle, "TotalBytesPerFrame", &mFrameSize);
        if (mErrcode != ePvErrSuccess)
        printf("Errcode: %d Could not open camera: \n", mErrcode);
        printf("Frame size \t\t= %d\n", mFrameSize);

        // Allocate a buffer to store the image
        memset(&mCameraFrame, 0, sizeof(tPvFrame));
        mCameraFrame.ImageBufferSize = mFrameSize;
        mCameraFrame.ImageBuffer = cvAlloc(mFrameSize);

        // Get the width & height of the image
        PvAttrUint32Get(mCameraHandle, "Width", &mWidth);
        PvAttrUint32Get(mCameraHandle, "Height", &mHeight);
        // Get Bit depth, if needed
        unsigned long mBitDepth = 0;
        PvAttrUint32Get(mCameraHandle, "BitDepth", &mBitDepth);

        printf("Frame width \t\t= %d\n", mWidth);
        printf("Frame height \t\t= %d\n\n", mHeight);
        // printf("Frame BitDepth \t\t= %ld\n\n", mBitDepth);

        // Start camera
        mErrcode = PvCaptureStart(mCameraHandle);
        if (mErrcode != ePvErrSuccess)
        printf("Errcode: %d Could not start camera: \n", mErrcode);

        // Set the camera to capture continuously
        mErrcode = PvAttrEnumSet(mCameraHandle, "AcquisitionMode", "Continuous");
        if (mErrcode != ePvErrSuccess)
        printf("Errcode: %d Could not set acquisition mode: \n", mErrcode);

        //Start image acquisition
        //printf("starting the image acquisition ...\n");
        mErrcode = PvCommandRun(mCameraHandle, "AcquisitionStart");
        if (mErrcode != ePvErrSuccess)
        printf("Errcode: %d Could not start acquisition: \n", mErrcode);

                //Set Trigger mode to Freerun
        mErrcode = PvAttrEnumSet(mCameraHandle, "FrameStartTriggerMode", "Freerun");
        if (mErrcode != ePvErrSuccess)
        printf("Errcode: %d Could not set trigger mode: \n", mErrcode);
        if(mCameraHandle)
        fprintf(stderr, "Handle OK\n");

        // parse xml configuration and do some camera image setup
        // Set Area/Region of interest
        tPvUint32 aoix = GetConfigurationInt("AOIOffset.x", 0);
        tPvUint32 aoiy = GetConfigurationInt("AOIOffset.y", 0);
        tPvUint32 aoiw = GetConfigurationInt("AOISize.x", MAX_IMG_W);
        tPvUint32 aoih = GetConfigurationInt("AOISize.y", MAX_IMG_H);
            if (aoix > mWidth - 1) {
                aoix = mWidth - 1;
            }
            if (aoiy > mHeight - 1) {
                aoiy = mHeight - 1;
            }
            if (aoix > mWidth) {
                aoix = mWidth;
            }
            if (aoiy > mHeight) {
                aoiy = mHeight;
            }

            if (aoiw > mWidth - aoix) {
                aoiw = mWidth - aoix;
            }
            if (aoih > mHeight - aoiy) {
                aoih = mHeight - aoiy;
            }
            aoiw = aoiw ^ (aoiw & 0x3);	// The image width must currently be a multiple of 4, for alignment reasons.
            aoih = aoih ^ (aoih & 0x3);	// The image height must currently be a multiple of 4.
            if (aoiw < MIN_IMG_W) { // minimum width forced
                aoiw = MIN_IMG_W;
            }
            if (aoih < MIN_IMG_H) { // minimum height forced
                aoih = MIN_IMG_H;
            }

            PvAttrUint32Set(mCameraHandle, "RegionX", aoix);
            PvAttrUint32Set(mCameraHandle, "RegionY", aoiy);
            PvAttrUint32Set(mCameraHandle, "Width", aoiw);
            PvAttrUint32Set(mCameraHandle, "Height", aoih);

            // Get the width & height of the image and update
            PvAttrUint32Get(mCameraHandle, "Width", &mWidth);
            PvAttrUint32Get(mCameraHandle, "Height", &mHeight);

        // Set Trigger Mode, if nessary, Unverified
        /*
        if (mTriggerMode == sTrigger_Software)
        {
          mErrcode = PvAttrEnumSet(mCameraHandle, "FrameStartTriggerMode", "Software");

        } else if (mTriggerMode == sTrigger_SyncIn1)
        {
                mErrcode = PvAttrEnumSet(mCameraHandle, "FrameStartTriggerMode", "SyncIn1");
        } else if (mTriggerMode == sTrigger_SyncIn2)
        {
                mErrcode = PvAttrEnumSet(mCameraHandle, "FrameStartTriggerMode", "SyncIn2");
        } else if (mTriggerMode == sTrigger_SyncIn3)
        {
                mErrcode = PvAttrEnumSet(mCameraHandle, "FrameStartTriggerMode", "SyncIn3");
        } else if (mTriggerMode == sTrigger_SyncIn4)
        {
                mErrcode = PvAttrEnumSet(mCameraHandle, "FrameStartTriggerMode", "SyncIn4");
        } else if (mTriggerMode == sTrigger_FixedRate)
        {
                mErrcode = PvAttrEnumSet(mCameraHandle, "FrameStartTriggerMode", "FixedRate");
        } else {
                mTriggerMode = sTrigger_Freerun;
                mErrcode = PvAttrEnumSet(mCameraHandle, "FrameStartTriggerMode", "Freerun");
        }
        */

    }

    // Create necessary image headers and images
    // Create only header for raw bayer image
    mBayerImage = cvCreateImageHeader(cvSize((int)mWidth, (int)mHeight), IPL_DEPTH_8U, CH_MONO);
    if (! mBayerImage) {
        printf("Failed to retrieve an item from the frame buffer: %s", mErrcode);
        return;
    } else {
        //printf("mBayerImage OK\n");
    }
    // Point bayer image to right buffer
    mBayerImage->widthStep = (int)mWidth;
    mBayerImage->imageData = (char *)mCameraFrame.ImageBuffer;

    // Create color image
    mColorImage  = cvCreateImage(cvSize((int)mWidth, (int)mHeight), IPL_DEPTH_8U, CH_COLOR);
    if (!mColorImage) {
        printf("Failed to create mColorImage Error: %s", mErrcode);
        return;
    } else {
        //printf("mColorImage OK\n");
    }
    //Create gray image
    mGrayImage  = cvCreateImage(cvSize((int)mWidth, (int)mHeight), IPL_DEPTH_8U, CH_MONO);
    if (!mGrayImage) {
        printf("Failed to create mGrayImage Error: %s", mErrcode);
        return;
    } else {
        //printf("mGrayImage OK\n");
    }

	// Configure reloadable values
	OnReloadConfiguration();


	// Prepare the trigger for the next frame

	if (mTriggerMode == sTrigger_Software) {
		// When using the software trigger, we are immediately ready to read the next image
		mTrigger->SetReady();
	} else {
		// Otherwise, restart a thread waiting for the next image
		ComponentInputCameraGigE::Thread *ct = new ComponentInputCameraGigE::Thread(this);
		ct->Create();
		ct->Run();
	}

	//fprintf(stderr, "End of start:\n");
}

void THISCLASS::OnReloadConfiguration() {

	if (mCameraHandle)
	{
	    //Set the exposure mode: auto, manual, auto once etc. external mode
        mExposureMode = (eExposureMode)GetConfigurationInt(wxT("ExposureMode"), 0);

        if(mExposureMode == sExposure_Manual)
        {
            mErrcode = PvAttrEnumSet(mCameraHandle, "ExposureMode", wxT("Manual"));
            mExposureValue = GetConfigurationInt(wxT("ExposureValue"), 6000);
            PvAttrUint32Set(mCameraHandle, "ExposureValue", mExposureValue);
        } else if (mExposureMode == sExposure_AutoOnce) {
            mErrcode = PvAttrEnumSet(mCameraHandle, "ExposureMode", wxT("AutoOnce"));
        } else if (mExposureMode == sExposure_Auto){
            mErrcode = PvAttrEnumSet(mCameraHandle, "ExposureMode", wxT("Auto"));
        } else if (mExposureMode == sExposure_External){
            mErrcode = PvAttrEnumSet(mCameraHandle, "ExposureMode", wxT("External"));
        }

        //Set the gain
        mAnalogGain = GetConfigurationInt(wxT("AnalogGain"), 0);
        PvAttrUint32Set(mCameraHandle, "GainValue", mAnalogGain);


        //Set something else! See PVAPI SDK manual
        mColor = GetConfigurationBool(wxT("Color"), false);
        if(!mColor){
            //printf("OnReloadConfiguration(): using mono 8bit mode\n");
        }
	}
	if(mErrcode != ePvErrSuccess)
	{
        printf("OnReloadConfiguration(): PV Error %d \n", mErrcode );
	}
    //printf("OnReloadConfiguration(): All functions are not implemented yet");
	return;
}

void THISCLASS::OnStep() {

    // In case of sTrigger_Software
	if (mTriggerMode == sTrigger_Software)
	{
		// Send the software trigger
		PvCommandRun(mCameraHandle, "Software");
	}

	// Check the image first returned by thread -- skipped
    // Convert image bayer8 -> color -> gray then link
    cvCvtColor(mBayerImage, mColorImage,  CV_BayerGB2BGR); // for direct cleaner output
    cvCvtColor(mColorImage,mGrayImage,CV_BGRA2GRAY);
	if(!mColor){
    	//mCore->mDataStructureInput.mImage = mBayerImage; //RAW bayer8 output, blob detection takes long time
    	mCore->mDataStructureInput.mImage = mGrayImage; // Clean Gray output, fast blob detetcion
      //mCore->mDataStructureImageGray.mImage = mGrayImage; //optional, for direct output
	} else { // In case we only need color image
		//mCore->mDataStructureInput.mImage = mBayerImage; //RAW bayer8 output
		mCore->mDataStructureInput.mImage = mColorImage; // Clean Color output, need to filter by other component
		//mCore->mDataStructureImageColor.mImage = mColorImage; //optional for direct output
	}

    // Set the frame number
    mFrameNumber++;
    mCore->mDataStructureInput.mFrameNumber = mFrameNumber;
}

void THISCLASS::OnStepCleanup() {

	// Prepare the trigger for the next frame
	if (mTriggerMode == sTrigger_Software) {
		// When using the software trigger, we are immediately ready to read the next image
		mTrigger->SetReady();
	} else {
		// Otherwise, restart a thread waiting for the next image
		ComponentInputCameraGigE::Thread *ct = new ComponentInputCameraGigE::Thread(this);
		ct->Create();
		ct->Run();
	}
}

void THISCLASS::OnStop()
{
  //printf("Closing starts\n.");
  mErrcode = PvCommandRun(mCameraHandle, "AcquisitionStop");
  if (mErrcode != ePvErrSuccess)
    fprintf(stderr, "Errcode: %d Could not stop video source: \n", mErrcode);

  // wait for thread to close
  wxCriticalSectionLocker csl(mThreadCriticalSection);

  PvCaptureEnd(mCameraHandle);
  PvCameraClose(mCameraHandle);

  cvFree(&(mCameraFrame.ImageBuffer));
  printf("Camera closed \n");

  // Releases memory
  if (mBayerImage) {
      cvReleaseImageHeader(&mBayerImage);
  }

  	if (mColorImage) {
		cvReleaseImage(&mColorImage);
	}

    if (mGrayImage) {
		cvReleaseImage(&mGrayImage);
	}
}

wxThread::ExitCode THISCLASS::Thread::Entry() {
	wxCriticalSectionLocker csl(mComponent->mThreadCriticalSection);

	// Wait for the grabbed image
	if(!PvCaptureQueueFrame(mComponent->mCameraHandle, &(mComponent->mCameraFrame), NULL))
    {
        //printf("waiting for the frame to be done...\n");
        while(PvCaptureWaitForFrameDone(mComponent->mCameraHandle, &(mComponent->mCameraFrame), 1000) == ePvErrTimeout)
        {    //printf("still waiting ...\n");
            ;
        }
        //printf("capture done\n");

    } else {
      printf("failed to capture\n");
      return 0;
    }
    //Sleep(5);
	mComponent->mTrigger->SetReady();
	return 0;
}
#endif
// ====================== END Prosilica Stuff =======================
