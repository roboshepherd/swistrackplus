#include "ComponentDBusServer.h"
#define THISCLASS ComponentDBusServer

#include "DisplayEditor.h"

THISCLASS::ComponentDBusServer(SwisTrackCore *stc):
		Component(stc, wxT("DBusServer")), mBgImage(0),
		mDisplayOutput(wxT("Output"), wxT("After DBusServer worked")) {

	// Data structure relations
	mCategory = &(mCore->mCategoryOutput);
	AddDataStructureRead(&(mCore->mDataStructureRobotDevices));
	AddDataStructureWrite(&(mCore->mDataStructureShopTasks));
	AddDisplay(&mDisplayOutput);

	// Read the XML configuration file
	Initialize();
}

THISCLASS::~ComponentDBusServer() {
}

void THISCLASS::OnStart() {
	OnReloadConfiguration();
}

void THISCLASS::OnReloadConfiguration() {
  if(mCore->mDataStructureImageGray.mImage){
        mBgImage = mCore->mDataStructureImageGray.mImage;
    }
}

void THISCLASS::OnStep() {

	// Set the display
	DisplayEditor de(&mDisplayOutput);
	if (de.IsActive()) {
		de.SetParticles(mCore->mDataStructureParticles.mParticles);
		de.SetMainImage(mBgImage);
	}
}

void THISCLASS::OnStepCleanup() {

}

void THISCLASS::OnStop() {

}
