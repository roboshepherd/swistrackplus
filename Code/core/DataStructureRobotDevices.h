#ifndef HEADER_DataStructureRobotDevices
#define HEADER_DataStructureRobotDevices

#include <vector>
#include <cv.h>
#include "DataStructure.h"
#include "RobotDevice.h"

// A DataStructure holding a list of Robot Devices.
class DataStructureRobotDevices: public DataStructure {

public:
	//! Particle vector type.
	typedef std::vector<RobotDevice> tRobotDeviceVector;

	tRobotDeviceVector *mRobotDevices;	//!< Vector of Robot Devices.

	//! Constructor.
	DataStructureRobotDevices(): DataStructure(wxT("RobotDevices"), wxT("Robots")), mRobotDevices(0) {}
	//! Destructor.
	~DataStructureRobotDevices() {}
};

#endif


