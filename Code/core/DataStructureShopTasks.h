
#ifndef HEADER_DataStructureShopTasks
#define HEADER_DataStructureShopTasks

#include <vector>
#include <cv.h>
#include "DataStructure.h"
#include "ShopTask.h"

// A DataStructure holding a list of ShopTasks.
class DataStructureShopTasks: public DataStructure {

public:
	//! ShopTask vector type.
	typedef std::vector<ShopTask> tShopTaskVector;

	tShopTaskVector *mShopTasks;	//!< Vector of ShopTasks.

	//! Constructor.
	DataStructureShopTasks(): DataStructure(wxT("ShopTasks"), wxT("Tasks")), mShopTasks(0) {}
	//! Destructor.
	~DataStructureShopTasks() {}
};

#endif

