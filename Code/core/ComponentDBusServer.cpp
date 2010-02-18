#include "ComponentDBusServer.h"
#define THISCLASS ComponentDBusServer

// shortcuts
# define STR_LEN 16
//#define MC_DS_ROBOTDEVICES mCore->mDataStructureRobotDevices.mRobotDevices
#define  MC_DS_PARTICLES mCore->mDataStructureParticles.mParticles


THISCLASS::ComponentDBusServer(SwisTrackCore *stc):
		Component(stc, wxT("DBusServer")),
		mDBusConn(0),
    mDBusErr(),
    mDBusMsg(0),
    mDBusArgs(),
    mDBusArray(),
    mBusPath(),
    mTaskNeighbors(),
    mRobotPeers(),
    mStepCount(0),
		mBgImage(0),
		mDisplayOutput(wxT("Output"), wxT("After DBusServer worked")) {

	// Data structure relations
	mCategory = &(mCore->mCategoryOutput);
	AddDataStructureRead(&(mCore->mDataStructureParticles));
	AddDisplay(&mDisplayOutput);

	// Read the XML configuration file
	Initialize();

	dbus_error_init (&mDBusErr);
    // get DBus connection
  mDBusConn = dbus_bus_get (DBUS_BUS_SESSION, &mDBusErr);
   if (dbus_error_is_set(&mDBusErr)) {
      fprintf(stderr, "Connection Error (%s)\n", mDBusErr.message);
      dbus_error_free(&mDBusErr);
   } else {
      printf ("Connected to the D-BUS daemon: \n");
  }

}

THISCLASS::~ComponentDBusServer() {
}

void THISCLASS::OnStart() {
	OnReloadConfiguration();
}

void THISCLASS::OnReloadConfiguration() {
  if(mCore->mDataStructureImageGray.mImage){
        mBgImage = mCore->mDataStructureImageGray.mImage;
    }

}

void THISCLASS::EmitRobotPeerList()
{
  DataStructureParticles::tParticleVector *r = MC_DS_PARTICLES;
  DataStructureParticles::tParticleVector::iterator it = r->begin();
   DataStructureParticles::tParticleVector::iterator it2 = r->begin();
  int myID, peerID;
  double x1, y1, x2, y2;

  while(it2 != r->end()) {
    mRobotPeers.clear();
    myID = it2->mID;
    x1 = it2->mCenter.x;
    y1 = it2->mCenter.y;
    printf("DBusServer now signalling peers of Robot %d... \n", myID);
    // find dist from all robots
    it = r->begin(); // reset
    //while(it != r->end()){
    for(unsigned i=0; i < r->size(); i++ ) {
      peerID = it->mID;
      x2 = it->mCenter.x;
      y2 = it->mCenter.y;
      //if(myID == peerID)
        //continue;
      if(InsideTargetRadius(x1, y1, x2, y2, (double )ROBOT_PEER_RADIUS))
        mRobotPeers.push_back(peerID);
      it++;
    }

    // remove self id
    mRobotPeers.erase(std::remove(mRobotPeers.begin(), mRobotPeers.end(),\
     myID), mRobotPeers.end());

    // prepare/send dbus msg
    if(mRobotPeers.size() > 0) {
      mBusPath =  wxT(DBUS_ROBOT_PATH_BASE) + wxString::Format(wxT("%d"), myID);

      mDBusMsg = dbus_message_new_signal (mBusPath, DBUS_IFACE, DBUS_SIGNAL_PEERS);
      if (mDBusMsg == NULL) { fprintf(stderr, "DBus Message Null\n"); }


      // Init iter for append
      dbus_message_iter_init_append(mDBusMsg, &mDBusArgs);

      // apend robotid
      if (!dbus_message_iter_append_basic(&mDBusArgs, DBUS_TYPE_INT32, &myID)) {
          fprintf(stderr, "DBus Out Of Memory for x!\n");
        }
      // append peers
      wxChar type_sig[2] = { DBUS_TYPE_INT32, '\0' };
      if(!dbus_message_iter_open_container(&mDBusArgs, DBUS_TYPE_ARRAY,\
       type_sig, &mDBusArray)){
          fprintf(stderr, "DBus can't open iter container!\n");
      }
      for(unsigned i =0; i < mRobotPeers.size();  i++) {
        int val = mRobotPeers.at(i);
        if (!dbus_message_iter_append_basic(&mDBusArray, DBUS_TYPE_INT32, &val)) {
          fprintf(stderr, "DBus Out Of Memory for adding value!\n");
        }
      }
      dbus_message_iter_close_container(&mDBusArgs, &mDBusArray);

      // emit signal
      dbus_connection_send (mDBusConn, mDBusMsg, NULL);
    }  else { //prepare/send dbus msg end
      printf("\tRobot %d has no neighbors! \n", myID);
    }
    ++it2;
  } // robot loop end
}

void THISCLASS::EmitTaskNeighborList()
{
  DataStructureParticles::tParticleVector *r = MC_DS_PARTICLES;
  DataStructureParticles::tParticleVector::iterator it = r->begin();
  int taskid, robotid;
  double tx, ty, rx, ry;

  for(int i=0; i < MAXSHOPTASK ; i++) {
    mTaskNeighbors.clear();
    taskid = i+1;
    printf("DBusServer Task %d: now signalling neighbors... \n", taskid);
    tx = TASKS_CENTERS[i][0];
    ty = TASKS_CENTERS[i][1];
    printf("Task x: %.0f, y:%.0f", tx, ty);
    // find dist from all robots
    it = r->begin(); // reset
    while(it != r->end()){
      robotid = it->mID;
      rx = it->mCenter.x;
      ry = it->mCenter.y;
      if(InsideTargetRadius(tx, ty, rx, ry, (double )TASK_NEIGHBOR_RADIUS))
        mTaskNeighbors.push_back(robotid);
      it++;
    }
    // prepare/send dbus msg
    if(mTaskNeighbors.size() > 0) {
      mBusPath =  wxT(DBUS_TASK_PATH_BASE) + wxString::Format(wxT("%d"), taskid);

      mDBusMsg = dbus_message_new_signal (mBusPath, DBUS_IFACE, DBUS_SIGNAL_TASK);
      if (mDBusMsg == NULL) { fprintf(stderr, "DBus Message Null\n"); }

      // Init iter for append
      dbus_message_iter_init_append(mDBusMsg, &mDBusArgs);

      // apend taskid
      if (!dbus_message_iter_append_basic(&mDBusArgs, DBUS_TYPE_INT32, &taskid)) {
          fprintf(stderr, "DBus Out Of Memory for x!\n");
        }
      // append neighbors
      wxChar type_sig[2] = { DBUS_TYPE_INT32, '\0' };
      if(!dbus_message_iter_open_container(&mDBusArgs, DBUS_TYPE_ARRAY,\
       type_sig, &mDBusArray)){
          fprintf(stderr, "DBus can't open iter container!\n");
      }
      for(unsigned i =0; i < mTaskNeighbors.size();  i++) {
        int val = mTaskNeighbors.at(i);
        if (!dbus_message_iter_append_basic(&mDBusArray, DBUS_TYPE_INT32, &val)) {
          fprintf(stderr, "DBus Out Of Memory for adding value!\n");
        }
      }
      dbus_message_iter_close_container(&mDBusArgs, &mDBusArray);

      // emit signal
      dbus_connection_send (mDBusConn, mDBusMsg, NULL);
    }  else { //prepare/send dbus msg end
      printf("\tTask %d has no neighbors! \n", taskid);
    }

  } // task loop end
}


void THISCLASS::EmitRobotPoses()
{
  DataStructureParticles::tParticleVector *r = MC_DS_PARTICLES;
  DataStructureParticles::tParticleVector::iterator it = r->begin();
  while(it != r->end()){
      printf("RobotDevice  %d: now signalling pose... \n", it->mID);
      int id = it->mID;
      double x = it->mCenter.x;
      double y = it->mCenter.y;
      double theta = it->mOrientation;
      mBusPath =  wxT(DBUS_ROBOT_PATH_BASE) + wxString::Format(wxT("%d"), id);

      printf("Path: %s \n", mBusPath.c_str());

      // send a DBus signal
      mDBusMsg = dbus_message_new_signal (mBusPath, DBUS_IFACE, DBUS_SIGNAL_POSE);
      if (mDBusMsg == NULL) { fprintf(stderr, "DBus Message Null\n"); }

        dbus_message_iter_init_append(mDBusMsg, &mDBusArgs);
      if (!dbus_message_iter_append_basic(&mDBusArgs, DBUS_TYPE_DOUBLE, &x)) {
        fprintf(stderr, "DBus Out Of Memory for x!\n");
      }

      if (!dbus_message_iter_append_basic(&mDBusArgs, DBUS_TYPE_DOUBLE, &y)) {
        fprintf(stderr, "DBus Out Of Memory for y!\n");
      }

      if (!dbus_message_iter_append_basic(&mDBusArgs, DBUS_TYPE_DOUBLE, &theta)) {
        fprintf(stderr, "DBus Out Of Memory for theta!\n");
      }
      /* Send the message */
      dbus_connection_send (mDBusConn, mDBusMsg, NULL);
      it++;
  }
}

void THISCLASS::OnStep() {
  mStepCount++;
  printf("\n ---------------- DBUS Server Step Start %d ------------\n", mStepCount);


  //DataStructureRobotDevices::tRobotDeviceVector *r = MC_DS_ROBOTDEVICES;
  DataStructureParticles::tParticleVector *r = mCore->mDataStructureParticles.mParticles;
  if (!r) {
    printf("RobotDevices not created yet\n");
    return;
	} else {
      EmitRobotPoses();
      EmitTaskNeighborList();
      // Emit at reduce freq
      if(!(mStepCount % SIGNAL_REDUCER_MOD)) {
        EmitRobotPeerList();
      }
  }




	// Set the display
	DisplayEditor de(&mDisplayOutput);
	if (de.IsActive()) {
		de.SetParticles(mCore->mDataStructureParticles.mParticles);
		de.SetMainImage(mBgImage);
	}
}

void THISCLASS::OnStepCleanup() {

  //dbus_message_unref (mDBusMsg);
}

void THISCLASS::OnStop() {
  mDBusConn = 0;
  dbus_message_unref (mDBusMsg);

}

bool THISCLASS::InsideTargetRadius(double tx, double ty,\
 double rx, double ry, double radius)
{
  bool ret = false;
  double dx = fabs(rx - tx);
  double dy = fabs(ry - ty);
  double pxdist = sqrt(dx * dx + dy * dy);
  //printf("pxdist: %f, pxdist\n");
  if (pxdist < radius) {
    ret = true;
  } else {
    ret = false;
  }

  return ret;
}
