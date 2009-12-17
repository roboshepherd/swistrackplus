# General
TARGET_FOLDER := $(SWISTRACK_ROOT)/SwisTrackEnvironment

# Toolchain
SHELL     := /bin/sh
CTAGS     := ctags
CC        := cc
CXX       := g++
AR        := ar

# OpenCV (activate the second set of lines in case you don't have pkg-config)
#OPENCV_CPPFLAGS    := `pkg-config --cflags opencv`
#OPENCV_LDLIBS      := `pkg-config --libs opencv`
OPENCV_CPPFLAGS    := -I/usr/include/opencv
OPENCV_LDLIBS      := -lcxcore -lcv -lhighgui -lcvaux -lml -lPvAPI

# wxWidgets
WXWIDGETS_CPPFLAGS := `wx-config --version=2.8 --cppflags`
WXWIDGETS_LDLIBS   := `wx-config --version=2.8 --libs`

# GiGE Camera Component (activate the following lines to compile with this component)
#PYLON_ROOT         := /opt/pylon
#GENICAM_ROOT_V1_1  := /opt/pylon
#CPPFLAGS_ADD       += -I$(PYLON_ROOT)/include -I$(GENICAM_ROOT_V1_1)/include/genicam -DUSE_CAMERA_PYLON_GIGE
#LDLIBS_ADD         += -L$(PYLON_ROOT)/lib -L$(GENICAM_ROOT_V1_1)/lib -lpylonbase

# XVID component (activate the following lines to compile with this component)
CPPFLAGS_ADD       += -DUSE_XVID
LDLIBS_ADD         += -lxvidcore

# Components that depend on the Boost library (activate the following lines to compile with these components)
#CPPFLAGS_ADD       += -DUSE_BOOST
#LDLIBS_ADD         += -lboost_graph

# Components that depend on the wxCTB library (activate the following lines to compile with these components)
CPPFLAGS_ADD       += -I/usr/local/include/wx
#LDLIBS_ADD         += -L/usr/local/lib -lwxctb-0.13

# Components that depend on boost interprocess library
CPPFLAGS_ADD       += -I/usr/local/include/boost
LDLIBS_ADD         += -L/usr/local/lib -lboost_signals-mt

# Components that depend on dbus-1 library
CPPFLAGS_ADD       += -I/usr/include/dbus-1.0 -I/usr/lib/dbus-1.0/include 
LDLIBS_ADD         += -L/lib -ldbus-1

