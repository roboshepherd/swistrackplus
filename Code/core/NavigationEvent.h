#ifndef HEADER_NavigationEvent
#define HEADER_NavigationEvent

#include <cv.h>
#include <wx/string.h>
#include <wx/datetime.h>

//! A NavigationEvent.
class NavigationEvent {

public:
	int mID;				//!< Referenc ID of event
    wxDateTime mTimestamp; //!< Time of occurance
    wxString mDesc;     //!< Description of event as per Data model

	//! Constructor.
	NavigationEvent(): mID(0), mTimestamp(), mDesc(){}
	//! Destructor.
	~NavigationEvent() {}


};

#endif
