#ifndef HEADER_DataStructureInputShopFloor
#define HEADER_DataStructureInputShopFloor

#include <wx/string.h>
#include <vector>
#include "DataStructureInput.h"
#include "DataStructureImage.h"
#include  "RectBlock.h"


class DataStructureInputShopFloor: public DataStructureInput
 {

public:
    int mMatSrcCount; // How many sources
    // Vector of Material Sources
    typedef std::vector<RectBlock*> tRectBlockVector;
    tRectBlockVector mMaterialSources;


    // Assembly Area
    RectBlock mAssemblyArea;

    // Finished Product Store
    //RectBlock mProductStore;

	//! Constructor.
 DataStructureInputShopFloor(const wxString &name, const wxString &displayname)
    : DataStructureInput(),
    mMatSrcCount(0),
    //mMaterialSources(new RectBlock()),
    mMaterialSources(0),
    mAssemblyArea(201, name, RectBlock::AssemblyArea, RectBlock::placeBottom2)
    //201, emptyname,, RectBlock::placeBottom3)
    //mProductStore(301, emptyname, RectBlock::ProductStore, RectBlock::placeBottom3)
    {
        mName = wxT("ShopFloor");
		mDisplayName = wxT("ShopFloor");

    }
	//! Destructor.
	~DataStructureInputShopFloor() {}

 private:


};

#endif
