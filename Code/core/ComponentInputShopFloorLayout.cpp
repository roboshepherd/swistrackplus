#include "ComponentInputShopFloorLayout.h"
#define THISCLASS ComponentInputShopFloorLayout
#define M_ASSEMBLY_AREA mCore->mDataStructureInputShopFloor.mAssemblyArea
#define M_MATERIAL_SRCS (mCore->mDataStructureInputShopFloor.mMaterialSources)

#define IMG_SEG 4 // for block placement purposes
#define OFFSET1 20
#define OFFSET2 30

#include <cv.h>
#include <highgui.h>
#include "DisplayEditor.h"

THISCLASS::ComponentInputShopFloorLayout(SwisTrackCore *stc):
		Component(stc, wxT("InputShopFloorLayout")),
		mBackgroundImage(0),
		mDisplayOutput(wxT("Output"), wxT("After ShopFloor Layout Updated")) {

	// Data structure relations
	mCategory = &(mCore->mCategoryOutput);
	AddDataStructureRead(&(mCore->mDataStructureImageGray));
	AddDataStructureWrite(&(mCore->mDataStructureInputShopFloor));
	AddDisplay(&mDisplayOutput);

	// Read the XML configuration file
	Initialize();
}

THISCLASS::~ComponentInputShopFloorLayout() {
}

void THISCLASS::OnStart() {

	// Read the reloadable parameters
	OnReloadConfiguration();



}

void THISCLASS::OnReloadConfiguration() {

    // Read Configuration:  Assembly Area Setup
    M_ASSEMBLY_AREA.mLabel = GetConfigurationString(wxT("AssemblyAreaLabel"), wxT("Box Assembly"));
    M_ASSEMBLY_AREA.mPlace= (RectBlock::placeType) GetConfigurationInt(wxT("AssemblyAreaPlace"), 18);
            //printf("place: %d \n", M_ASSEMBLY_AREA.mPlace);
    M_ASSEMBLY_AREA.mBlockSize.width= GetConfigurationDouble(wxT("BlockWidth"), 530.0);
    M_ASSEMBLY_AREA.mBlockSize.height=GetConfigurationDouble(wxT("BlockHeight"), 330.0);


    // Load the background image
    //mBackgroundImage = mCore->mDataStructureImageColor.mImage;
    mBackgroundImage = mCore->mDataStructureImageGray.mImage;

//	if (! mBackgroundImage) {
//		AddError(wxT("No input image."));
//		return;
//	}

}

void THISCLASS::OnStep() {
	// Get and check input image
	//mBackgroundImage = mCore->mDataStructureImageColor.mImage;
//    if (! mBackgroundImage) {
//		AddError(wxT("Cannot open background image."));
//		return;
//	}
        OnReloadConfiguration();


    // Draw a rectangle
    if ( mBackgroundImage) {
        wxString labelAA = M_ASSEMBLY_AREA.mLabel;
        int place = M_ASSEMBLY_AREA.mPlace;
        int x = ((mBackgroundImage->width)/IMG_SEG) * (place % IMG_SEG);
        int y = mBackgroundImage->height-OFFSET1; // little offset from bottom
        int w = M_ASSEMBLY_AREA.mBlockSize.width;
        int h = M_ASSEMBLY_AREA.mBlockSize.height;
        //printf("x-y-w-h: %d-%d %d-%d\n", x,y,w,h);
        CvPoint pt1 = cvPoint(x-w,y-h);
        CvPoint pt2 = cvPoint(x,y);
        // Draw the rectangle in the input image
        cvRectangle( mBackgroundImage, pt1, pt2, CV_RGB(255,255,0), 8, 8, 0);
        // Mark it
        CvFont font;
        double hScale=2.0;
        double vScale=2.0;
        int    lineWidth=2.0;
        cvInitFont(&font,CV_FONT_HERSHEY_COMPLEX, hScale,vScale,0,lineWidth);
        cvPutText (mBackgroundImage,labelAA, cvPoint(pt1.x+OFFSET2, pt1.y+h/2), &font, CV_RGB(255, 0,0));

    // Read Configuration:  Task Area Setup
    mCore->mDataStructureInputShopFloor.mMatSrcCount =\
      GetConfigurationInt(wxT("MaterialSources"), 3);
    // parse configuration and add all material sources
    wxString labelS1 = GetConfigurationString(wxT("SourceLabel1"), wxT("Top part"));
    RectBlock::placeType placeS1 = (RectBlock::placeType) GetConfigurationInt(wxT("SourcePlace1"), 6);
    M_MATERIAL_SRCS.push_back(new RectBlock(1, labelS1, RectBlock::MaterialSources, placeS1));

    wxString labelS2 = GetConfigurationString(wxT("SourceLabel2"), wxT("Side part"));
    RectBlock::placeType placeS2 = (RectBlock::placeType) GetConfigurationInt(wxT("SourcePlace2"), 10);
    M_MATERIAL_SRCS.push_back(new RectBlock(1, labelS2, RectBlock::MaterialSources, placeS2));

    wxString labelS3 = GetConfigurationString(wxT("SourceLabel3"), wxT("Bottom part"));
    RectBlock::placeType placeS3 = (RectBlock::placeType) GetConfigurationInt(wxT("SourcePlace3"), 14);
    M_MATERIAL_SRCS.push_back(new RectBlock(1, labelS3, RectBlock::MaterialSources, placeS3));

    // Draw material sources top
    //printf("PlaceS1 2 3: %d - %d - %d \n", placeS1, placeS2, placeS3);
    int x2 = ((mBackgroundImage->width)/IMG_SEG) * (placeS3 % IMG_SEG);
    int y2 = OFFSET1; // little offset from bottom
    CvPoint pt21 = cvPoint(x2-w,y2+h);
    CvPoint pt22 = cvPoint(x2,y2);
    // Draw the rectangle in the input image
    cvRectangle( mBackgroundImage, pt21, pt22, CV_RGB(255,127,0), 8, 8, 0);
    cvPutText (mBackgroundImage,labelS3, cvPoint(pt21.x+OFFSET2, pt21.y-h/2), &font, CV_RGB(255, 0,0));

    ///TODO: Store pt21 and pt22 for locating ShopTask

    // Draw material source at left
    int y3 = ((mBackgroundImage->height)/IMG_SEG) * (placeS1 % IMG_SEG);
    int x3 = OFFSET1; // little offset from bottom
    CvPoint pt31 = cvPoint(x3+w,y3+h);
    CvPoint pt32 = cvPoint(x3,y3);
    // Draw the rectangle in the input image
    cvRectangle( mBackgroundImage, pt31, pt32, CV_RGB(255,127,0), 8, 8, 0);
    cvPutText (mBackgroundImage,labelS1, cvPoint(pt32.x+OFFSET2, pt32.y+h/2), &font, CV_RGB(255, 0,0));

     // Draw material source at right
    int y4 = ((mBackgroundImage->height)/IMG_SEG) * (placeS2 % IMG_SEG);
    int x4 = mBackgroundImage->width - OFFSET1; // little offset from bottom
    CvPoint pt41 = cvPoint(x4-w,y4+h);
    CvPoint pt42 = cvPoint(x4,y4);
    // Draw the rectangle in the input image
    cvRectangle( mBackgroundImage, pt41, pt42, CV_RGB(255,127,0), 8, 8, 0);
    cvPutText (mBackgroundImage,labelS2, cvPoint(pt41.x+OFFSET2, pt41.y-h/2), &font, CV_RGB(255, 0,0));


    }

    // add drawn image
     mCore->mDataStructureInputShopFloor.mImage= mBackgroundImage;

	// Set the display
	DisplayEditor de(&mDisplayOutput);
	if (de.IsActive()) {
	    de.SetParticles(mCore->mDataStructureParticles.mParticles);
		de.SetMainImage(mBackgroundImage);
	}
}

void THISCLASS::OnStepCleanup() {
     mCore->mDataStructureInputShopFloor.mImage = 0;
}

void THISCLASS::OnStop() {
//	if (mBackgroundImage) {
//		cvReleaseImage(&mBackgroundImage); //FIXME?
//	}
}
