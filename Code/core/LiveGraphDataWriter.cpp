#include "LiveGraphDataWriter.h"
#define THISCLASS LiveGraphDataWriter

#include <ctime>
#include <time.h>
#include <cstring>
#include <string>

void THISCLASS::SetTimeStamp()
{
  mTime = NULL;
  int rc;
  time_t temp;
  struct tm *timeptr;
  char s[100];
  temp = time(NULL);
  timeptr = localtime(&temp);

  rc = strftime (s, sizeof(s),"%Y%m%d-%H%M",timeptr);
  mTime = s;
  strcpy(mTimeNow, s);
  //printf("mTime set as %s\n", mTime);
}

void THISCLASS::GetTimeStamp(std::string &ts)
{
  SetTimeStamp();
  //printf("mTime now: %s str size: %d\n", mTimeNow, strlen(mTime)+1);
  ts = std::string(mTimeNow);
}

std::string THISCLASS::GetTimeStamp()
{

  SetTimeStamp();
  //printf("mTime now: %s str size: %d\n", mTimeNow, strlen(mTimeNow));
  return std::string(mTimeNow);
}


void THISCLASS::GetDataSeparator(std::string &s)
{
  s = mDataCtx->sep;
}

void THISCLASS::InitDataFile(std::string& objtype, int id, DataContextType *ctx)
{
  //update mTime
  SetTimeStamp();

  mObjType = objtype;
  mObjID = id;
  mDataCtx = ctx;
  sprintf(mFileName, "%s%d-%s-%s.txt", mObjType.c_str(), id,\
   mDataCtx->name.c_str(), mTime);
  mFileStream.open(mFileName, std::ios::in | std::ios::out | std::ios::app);
  mFileStream << "##" << mDataCtx->sep  << "##" << std::endl;
  mFileStream << "@" << mDataCtx->desc  << std::endl;
  mFileStream << mDataCtx->label  << std::endl;
  mFileStream.close();
}


// Open file stream, add separator, description, data label
void THISCLASS::InitDataFile(std::string& objtype, DataContextType *ctx)
{
  //update mTime
  SetTimeStamp();

  mObjType = objtype;
  mDataCtx = ctx;
  sprintf(mFileName, "%s-%s-%s.txt", mObjType.c_str(),\
   mDataCtx->name.c_str(), mTime);
  mFileStream.open(mFileName, std::ios::in | std::ios::out | std::ios::app);
  mFileStream << "##" << mDataCtx->sep  << "##" << std::endl;
  mFileStream << "@" << mDataCtx->desc  << std::endl;
  mFileStream << mDataCtx->label  << std::endl;
  mFileStream.close();

}

// Append Data at the end of file stream
void THISCLASS::AppendData(std::string& data)
{
  mFileStream.open( mFileName , std::ios::in | std::ios::out | std::ios::app);
  mFileStream << data << std::endl;
  mFileStream.close();
}

// Append comment at the end of file stream
void THISCLASS::AppendComment(std::string& comment)
{

  mFileStream.open( mFileName , std::ios::in | std::ios::out | std::ios::app);
  mFileStream << "# " << comment << std::endl;
  mFileStream.close();
}

