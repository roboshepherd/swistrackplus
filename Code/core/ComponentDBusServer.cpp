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
  // get DBus connection
  dbus_error_init (&mDBusErr);
  mDBusConn = dbus_bus_get (DBUS_BUS_SESSION, &mDBusErr);
  if (!mDBusConn)
  {
      printf ("Failed to connect to the D-BUS daemon: %s", mDBusErr.message);
      dbus_error_free (&mDBusErr);
  }

}

void THISCLASS::OnStep() {

  // send a DBus signal
  DBusMessage *message;

  message = dbus_message_new_signal ("/org/share/linux",
                                     "org.share.linux",
                                     "Customize");
  /* Send the message */
  dbus_connection_send (mDBusConn, message, NULL);
  dbus_message_unref (message);



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
