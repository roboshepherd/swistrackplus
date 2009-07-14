#ifndef HEADER_LiveGraphDataWriter
#define HEADER_LiveGraphDataWriter

#include <cstdlib>
#include <fstream>
#include <time.h>
#include <string>

#define FILE_NAME_LEN 256
#define DATA_ITEM_LEN 128
#define DATA_SEP ";"
//! A Data logger class that preapres a data file for LiveGraph Java app, , see LiveGrapgh file format v1.1
class LiveGraphDataWriter {

public:

  typedef struct DataContext {
    std::string name; //! context name, separate name for each type of data file
    std::string sep;   //! for data values separator line, e.g. ##;##
    std::string desc; //! File information e.g. (at)FileDesc
    std::string label; //! Data column headings
  } DataContextType;


  std::string mObjType; //! type of object
  int mObjID; //! target object's id
  char *mTime;		//!< time stamp
  char mTimeNow[14];
  DataContextType *mDataCtx; //! Assocaiated context

  char mFileName[FILE_NAME_LEN]; //! generated filename from objid and ctx name
  std::fstream mFileStream; //! target file for a target object's target record

  //! ctor dtor
  LiveGraphDataWriter(): mObjType(), mObjID(),  mTimeNow(), mDataCtx(),  mFileStream() {}
  LiveGraphDataWriter(char* time): mTime(time) {}
  //! Destructor.
  ~LiveGraphDataWriter(){}

	// Task implementation methods
	//! Writes the file header.
	//! Open file stream, add separator, description, data label
	void InitDataFile(std::string& objtype, DataContextType *ctx);
	void InitDataFile(std::string& objtype, int id, DataContextType *ctx);
	void AppendData(std::string& data); //! Append Data at the end of file stream
  void AppendComment(std::string& comment); //! Append comment at the end of file stream

  void SetTimeStamp(void); //! time stamp for filename
  void GetTimeStamp(std::string &ts);
  void GetDataSeparator(std::string &s);
  std::string GetTimeStamp();
};

#endif


