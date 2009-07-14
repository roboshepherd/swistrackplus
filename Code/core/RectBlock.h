#ifndef HEADER_RectBlock
#define HEADER_RectBlock

#include <cv.h>
#include <wx/string.h>

//! A rectangular block located in the shop-floor.
class RectBlock {

public:
	// identity
	int mID;				//!< ID of the block
    wxString mLabel;         //!< (Configuration) Name of this block
    enum blockType {
        MaterialSources=0,
        AssemblyArea,
        ProductStore,
        blockNull,
    };
    blockType mBlockType;

    // spatial location
	CvPoint2D32f mOrigin;	//!< RectBlock origin [pixel]
	CvSize2D32f mBlockSize;  //!< (Configuration) RectBlock size [pixel]

	//CvPoint mPt1; //one diagonal point
	//CvPoint mPt2; //oposite point

	enum placeType {
        placeNull,   //!< (Configuration) Default place in image
	    placeTopLeft=1,     //!< Each image wall has been divided into four equal segments
	    placeBottomLeft,
        placeTopRight,
	    placeBottomRight,
	    placeLeft1=5,
	    placeLeft2,
	    placeLeft3,
	    placemeftN,
	    placeRight1=9,
	    placeRight2,
	    placeRight3,
	    placeRightN,
	    placeTop1=13,
	    placeTop2,
	    placeTop3,
	    placeTopN,
	    placeBottom1=17,
	    placeBottom2,
	    placeBottom3,
	    placeBottomN,
	};
    placeType mPlace;

	//! Constructor.
	RectBlock(int id = -1, const wxString &label = wxT(""),\
        blockType blocktype = RectBlock::blockNull,\
        placeType place= RectBlock::placeNull): mID(id), mLabel(label),\
        mBlockType(blocktype), mPlace(place) {}

    RectBlock( int id, blockType blocktype = RectBlock::blockNull,\
        placeType place= RectBlock::placeNull): mID(id), mBlockType(blocktype), mPlace(place) {}

    RectBlock(): mID(-1), mLabel(), mBlockType(),\
    mOrigin(), mBlockSize(), mPlace() {}
	//! Destructor.
	~RectBlock() {}
};

#endif

