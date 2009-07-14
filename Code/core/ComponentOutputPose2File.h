#ifndef HEADER_ComponentOutputPose2File
#define HEADER_ComponentOutputPose2File

#include <vector>
#include <map>
#include <cv.h>
#include <highgui.h>
#include <fstream>
#include "Component.h"

//! An output component writing the particle pose data in tab seperated files.
class ComponentOutputPose2File: public Component {

public:
	//! Constructor.
	ComponentOutputPose2File(SwisTrackCore *stc);
	//! Destructor.
	~ComponentOutputPose2File();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();
	Component *Create() {
		return new ComponentOutputPose2File(mCore);
	}

	static void WriteParticle(std::fstream & fileStream, const Particle & particle);


private:
	struct structOutputFile {
		std::fstream fileStream;
		int particleID;
	};

	// Map track ID -> file.
	typedef std::map<int, structOutputFile*> tFilesMap;

	wxString mDirectoryName;		//!< Name of the directory to save
	Display mDisplayOutput;			//!< The DisplayImage showing the output of this component.
	tFilesMap mFiles;				//!< Vector containing the FileName

	//! Writes the file header.
	static void WriteHeader(std::fstream & fileStream);
	//! Writes one data record.
	void WriteData(structOutputFile *outputFile);
};

#endif

