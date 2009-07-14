#ifndef HEADER_NavigationCommand
#define HEADER_NavigationCommand

#include <cv.h>
#include <wx/string.h>
#include <wx/datetime.h>

//! A NavigationCommand.
class NavigationCommand {

public:
	int mID;				//!< Referenc ID of Navigation command
    wxDateTime mTimestamp; //!< Time of occurance
    wxString mDesc;     //!< Description of Command as per Data model

	//! Constructor.
	NavigationCommand(): mID(0), mTimestamp(), mDesc(){}
	//! Destructor.
	~NavigationCommand() {}


};

#endif
