#include "ComponentDBusServer.h"
#define THISCLASS ComponentDBusServer

#include "DisplayEditor.h"

// shortcuts
#define MC_DS_ROBOTDEVICES mCore->mDataStructureRobotDevices.mRobotDevices
//#define  MC_DS_ROBOTDEVICES mCore->mDataStructureParticles.mParticles

THISCLASS::ComponentDBusServer(SwisTrackCore *stc):
		Component(stc, wxT("DBusServer")),
		mDBusConn(0),
    mDBusErr(),
    mDBusMsg(0),
    mDBusArgs(),
		mBgImage(0),
		mDisplayOutput(wxT("Output"), wxT("After DBusServer worked")) {

	// Data structure relations
	mCategory = &(mCore->mCategoryOutput);
	AddDataStructureRead(&(mCore->mDataStructureRobotDevices));
	AddDataStructureWrite(&(mCore->mDataStructureShopTasks));
	AddDisplay(&mDisplayOutput);

	// Read the XML configuration file
	Initialize();

	dbus_error_init (&mDBusErr);
    // get DBus connection
  mDBusConn = dbus_bus_get (DBUS_BUS_SESSION, &mDBusErr);
  if (!mDBusConn)
  {
      printf ("Failed to connect to the D-BUS daemon: %s", mDBusErr.message);
      dbus_error_free (&mDBusErr);
  }

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
  printf("\n ---------------- DBUS Server Step Start ------------\n");


//  DataStructureRobotDevices::tRobotDeviceVector *r = MC_DS_ROBOTDEVICES;
//  if (!r) {
//    printf("RobotDevices not created yet\n");
//    return;
//	} else {
//      DataStructureRobotDevices::tRobotDeviceVector::iterator it = r->begin();
//      while(it != r->end()){
//          printf("RobotDevice  %d: now signalling pose... \n", it->mID);
//          int id = it->mID;
//          char *path;
//          sprintf(path,"%s%d", DBUS_PATH_BASE, id);
//          // send a DBus signal
//          mDBusMsg = dbus_message_new_signal (path, DBUS_IFACE, DBUS_SIGNAL_POSE);
//          if (mDBusMsg == NULL) { fprintf(stderr, "DBus Message Null\n"); }
//
////          dbus_message_iter_init_append(mDBusMsg, &mDBusArgs);
////          if (!dbus_message_iter_append_basic(&mDBusArgs, DBUS_TYPE_STRING, &path)) {
////            fprintf(stderr, "DBus Out Of Memory!\n");
////          }
//
//          /* Send the message */
//          dbus_connection_send (mDBusConn, mDBusMsg, NULL);
//          it++;
//      }
//  }




	// Set the display
	DisplayEditor de(&mDisplayOutput);
	if (de.IsActive()) {
		de.SetParticles(mCore->mDataStructureParticles.mParticles);
		de.SetMainImage(mBgImage);
	}
}

void THISCLASS::OnStepCleanup() {

  //dbus_message_unref (mDBusMsg);
}

void THISCLASS::OnStop() {
  mDBusConn = 0;
  dbus_message_unref (mDBusMsg);

}
