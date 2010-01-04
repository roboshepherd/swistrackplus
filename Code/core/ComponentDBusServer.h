#ifndef HEADER_ComponentDBusServer
#define HEADER_ComponentDBusServer

#include <dbus/dbus.h>
#include <cv.h>
#include "Component.h"

#include <wx/string.h>
// D-BUs Defs
#define DBUS_IFACE "uk.ac.newport.ril.SwisTrack"
#define DBUS_SIGNAL_POSE "PoseSignal"
#define DBUS_PATH_BASE "/robot"

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

private:
  DBusConnection *mDBusConn;
  DBusError mDBusErr;
  DBusMessage *mDBusMsg;
  DBusMessageIter mDBusArgs;
  wxString mBusPath;
  IplImage *mBgImage;				//!< The image used by this component.
	Display mDisplayOutput;				//!< The DisplayImage showing the output of this component.

};

#endif

