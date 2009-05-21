ROOTLIBS:=$(shell root-config --libs)
ROOTINC:=-I$(shell root-config --incdir)
RATINC:=-I$(RATROOT)/include -I$(RATROOT)/src/stlplus
RATLIB:=-L$(RATROOT)/lib
TAINC:=-I./include
#For ROOTCINT
TOPOBJ:=include/TrackedOpticalPhoton.hpp
TOPLINKDEF:=include/LinkDef.h

#For ROOT Compatibility < 5.22
ROOTMJVER:=$(shell root-config --version | sed -e 's/\.[0-9]\{2\}\/[0-9]\{2\}//')
ROOTMNVER:=$(shell root-config --version | sed -e 's/5\.//' -e 's/\/[0-9]\{2\}//')

#TRACKANALYSIS DEFS
TALIBOBJ:=src/TrackedOpticalPhoton.cpp src/MCTrackHelper.cpp
TAOBJ:=src/ta.cpp
TALIB:=-L. -lTrackAnalysis
TABINNAME:=trackanalysis

#RATLIB DEFS
RATEVLIB:=RATEvent

#CXX DEFS
CXX:=g++
CXXFLAGS=-O2 -Wall
LIBFLAGS=
LIBNAME=TrackAnalysis

#HOST defs
HOSTSYS:=

#ROOT < v5.22 compatibility checks
ifeq ($(shell test `root-config --version | sed -e 's/5\.//' -e 's/\/[0-9]\{2\}//'` -lt 22 && echo 0),0)
CXXFLAGS+=-D__ROOTVERLT522
endif

#OSX v. Linux environment checks.
ifeq ($(shell uname),Darwin)
override HOSTSYS:=Darwin
LIBFLAGS+=-dynamiclib
override LIBNAME:=$(addprefix lib,$(LIBNAME))
override LIBNAME:=$(addsuffix .dylib,$(LIBNAME))
override RATEVLIB:=$(addsuffix _Darwin-g++,$(RATEVLIB))
else ifeq ($(shell uname),Linux)
override HOSTSYS:=Linux
CXXFLAGS+=-fPIC -m64
LIBFLAGS+=-shared
override LIBNAME:=$(addprefix lib,$(LIBNAME))
override LIBNAME:=$(addsuffix .so,$(LIBNAME))
override RATEVLIB:=$(addsuffix _Linux-g++,$(RATEVLIB))
endif

all: dictTOP.C libTA TAbin

dictTOP.C: $(TOPOBJ)
	rootcint dictTOP.C -c -p $(TOPOBJ) $(TOPLINKDEF)

libTA: dictTOP.C $(TALIBOBJ)
	$(CXX) $(CXXFLAGS) -o $(LIBNAME) $(LIBFLAGS) dictTOP.C $(RATINC) $(TAINC) $(TALIBOBJ) $(ROOTLIBS) $(ROOTINC) $(RATLIB) -l$(RATEVLIB)
ifeq ($(HOSTSYS),Darwin)
	ln -sf $(LIBNAME) $(LIBNAME:.dylib=.so) 
endif

TAbin: $(TAOBJ)
	$(CXX) $(CXXFLAGS) -o $(TABINNAME) $(TAINC) $(TALIB) $(TAOBJ) $(RATINC) $(ROOTINC) $(ROOTLIBS) $(RATLIB) -l$(RATEVLIB)

clean:
	rm dictTOP.* 

distclean:
	rm dictTOP.* libTrackAnalysis.* trackanalysis
