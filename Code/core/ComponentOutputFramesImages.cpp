#include "ComponentOutputFramesImages.h"
#define THISCLASS ComponentOutputFramesImages

#include "DisplayEditor.h"
#include <iostream>
#include <iomanip>
#include <sstream>
using namespace std;

THISCLASS::ComponentOutputFramesImages(SwisTrackCore *stc):
		Component(stc, wxT("OutputFramesImages")),
		mInputSelection(0),
		mDisplayOutput(wxT("Output"), wxT("FramesImages: Unprocessed Frame")) {

  //Font stuff
  cvInitFont(&mFontMain, CV_FONT_HERSHEY_COMPLEX, 1.5, 1.5, 0, 3, 8);

	// Data structure relations
	mCategory = &(mCore->mCategoryOutput);
	AddDisplay(&mDisplayOutput);
	AddDataStructureRead(&(mCore->mDataStructureImageGray));
	AddDataStructureRead(&(mCore->mDataStructureImageColor));
	AddDataStructureRead(&(mCore->mDataStructureImageBinary));

	// Read the XML configuration file
	Initialize();
}

THISCLASS::~ComponentOutputFramesImages() {
}

void THISCLASS::OnStart() {
	wxString filename_string = GetConfigurationString(wxT("FilePrefix"), wxT(""));
	mFileName=mCore->GetRunFileName(filename_string);
	// make sure that subdirectory exists:
	mFileName.Mkdir(mFileName.GetPath(), 0777, wxPATH_MKDIR_FULL);
	mFileType = (eFileType)GetConfigurationInt(wxT("FileType"), 0);
	OnReloadConfiguration();
}

void THISCLASS::OnReloadConfiguration() {
	mInputSelection = GetConfigurationInt(wxT("InputImage"), 0);
}

void THISCLASS::OnStep() {
	// Get the input image
	IplImage* inputimage;
	switch (mInputSelection) {
	case 0:
		// Gray image
		inputimage = mCore->mDataStructureImageGray.mImage;
		break;
	case 1:
		// Color image
		inputimage = mCore->mDataStructureImageColor.mImage;
		break;
	case 2:
		// Binary image
		inputimage = mCore->mDataStructureImageBinary.mImage;
		break;
	default:
		AddError(wxT("Invalid input image"));
		return;
	}

	if (! inputimage) {
		AddError(wxT("No image on selected input."));
		return;
	}
	char *fileExtension;
	switch (mFileType) {
	case 0:
		fileExtension = ".png";
		break;
	case 1:
		fileExtension = ".bmp";
		break;
	case 2:
		fileExtension = ".jpeg";
	}


    // save it
  std::ostringstream filename_oss;
	filename_oss << mFileName.GetFullPath().mb_str(wxConvFile) << "-" << std::setw(8) << std::setfill('0') << mCore->GetStepCounter() << fileExtension;
	cvSaveImage(filename_oss.str().c_str(), inputimage );

	// Image is always top down in Swistrack
	inputimage->origin = 0;

	// Set the display
	DisplayEditor de(&mDisplayOutput);
	if (de.IsActive()) {
		de.SetMainImage(inputimage);
		//de.SetParticles(mCore->mDataStructureParticles.mParticles);
	}
  // paint particle id's in input image
  /*DrawParticles(&mDisplayOutput, inputimage);  // Makes main loop slow */

}

void THISCLASS::DrawParticles(Display *mDisplay, IplImage* mImage )
{
  float mScalingFactor = 1.0;
  // Draw particles
	DataStructureParticles::tParticleVector::iterator it = mDisplay->mParticles.begin();
	while (it != mDisplay->mParticles.end()) {
		int x = (int)floor(it->mCenter.x * mScalingFactor + 0.5);
		int y = (int)floor(it->mCenter.y * mScalingFactor + 0.5);
		cvRectangle(mImage, cvPoint(x - 2, y - 2), cvPoint(x + 2, y + 2), cvScalar(192, 0, 0), 1);

		float c = cosf(it->mOrientation) * 8; //cosf(it->mOrientation/57.29577951)*20; // TODO: mOrientation contains an angle (rad), and the appropriate conversion to rad should be done when filling this structure
		float s = sinf(it->mOrientation) * 8; //sinf(it->mOrientation/57.29577951)*20;
		cvLine(mImage, cvPoint(x, y), cvPoint(x + (int)floorf(c + 0.5), y + (int)floorf(s + 0.5)), cvScalar(192, 0, 0), 1);

//		wxString label = wxString::Format(wxT("%d [%.0f,%.0f,%.2f]"),\
//		 it->mID, it->mCenter.x, it->mCenter.y, it->mOrientation);
    wxString label = wxString::Format(wxT("%d"), it->mID);
		cvPutText(mImage, label.mb_str(wxConvISO8859_1), cvPoint(x + 35, y - 35 ), &mFontMain, cvScalar(0, 0, 0));
		it++;
	}
}

void THISCLASS::OnStepCleanup() {
}

void THISCLASS::OnStop() {
}
