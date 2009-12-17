#ifndef HEADER_ComponentDBusServer
#define HEADER_ComponentDBusServer

#include <cv.h>
#include "Component.h"

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
  IplImage *mBgImage;				//!< The image used by this component.
	Display mDisplayOutput;				//!< The DisplayImage showing the output of this component.

};

#endif

