#ifndef HEADER_ComponentDBusServer
#define HEADER_ComponentDBusServer

#include <wx/string.h>
#include <dbus/dbus.h>
#include <cv.h>
#include <vector>
#include <algorithm>

#include "Component.h"
#include "DisplayEditor.h"

#include "RILObjects.h" // for task locations
#define TASK_NEIGHBOR_RADIUS 500 // pixel
#define ROBOT_PEER_RADIUS 300
// D-BUs Defs
#define DBUS_IFACE "uk.ac.newport.ril.SwisTrack"
#define DBUS_SIGNAL_POSE "PoseSignal"
#define DBUS_ROBOT_PATH_BASE "/robot"
#define DBUS_SIGNAL_TASK "TaskNeighbors"
#define DBUS_TASK_PATH_BASE "/task"
#define DBUS_SIGNAL_PEERS "RobotPeers"


//! A component that broadcast message over DBus
class ComponentDBusServer: public Component {

public:
	//! Constructor.
	ComponentDBusServer(SwisTrackCore *stc);
	//! Destructor.
	~ComponentDBusServer();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();
	Component *Create() {
		return new ComponentDBusServer(mCore);
	}

	void EmitRobotPoses();
	void EmitTaskNeighborList();
	void EmitRobotPeerList();
	bool InsideTargetRadius(double tx, double ty,\
        double rx, double ry, double radius);

private:
  DBusConnection *mDBusConn;
  DBusError mDBusErr;
  DBusMessage *mDBusMsg;
  DBusMessageIter mDBusArgs;
  wxString mBusPath;
  std::vector<int> mTaskNeighbors;
  std::vector<int> mRobotPeers;
  IplImage *mBgImage;				//!< The image used by this component.
	Display mDisplayOutput;				//!< The DisplayImage showing the output of this component.

};

#endif

