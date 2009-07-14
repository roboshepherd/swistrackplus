#ifndef HEADER_ComponentInputShopFloorLayout
#define HEADER_ComponentInputShopFloorLayout

#include <cv.h>
#include "Component.h"
#include "RectBlock.h"
#include "DataStructureInputShopFloor.h"


//! A component painting a virtual shop floor on grayscale image.
class ComponentInputShopFloorLayout: public Component {

public:
	//! Constructor.
	ComponentInputShopFloorLayout(SwisTrackCore *stc);
	//! Destructor.
	~ComponentInputShopFloorLayout();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();
	Component *Create() {
		return new ComponentInputShopFloorLayout(mCore);
	}

private:

	IplImage *mBackgroundImage;			//!< The background image.


	Display mDisplayOutput;				//!< The DisplayImage showing the output of this component.
};

#endif


