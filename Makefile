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
MACRATLIB:=RATEvent_Darwin-g++
LINRATLIB:=RATEvent_Linux-g++

#CXX DEFS
CXX:=g++
CXXFLAGS=-O2 -Wall
LIBFLAGS=
LIBNAME=TrackAnalysis

#ROOT < v5.22 compatibility checks
ifeq ($(shell test `root-config --version | sed -e 's/5\.//' -e 's/\/[0-9]\{2\}//'` -lt 22 && echo 0),0)
CXXFLAGS+=-D__ROOTVERLT522
endif

#OSX v. Linux environment checks.
ifeq ($(shell uname),Darwin)
LIBFLAGS+=-dynamiclib
override LIBNAME:=$(addprefix lib,$(LIBNAME))
override LIBNAME:=$(addsuffix .dylib,$(LIBNAME))
endif

all: dictTOP.C libTA TAbin

dictTOP.C: $(TOPOBJ)
	rootcint dictTOP.C -c -p $(TOPOBJ) $(TOPLINKDEF)

libTA: dictTOP.C $(TALIBOBJ)
ifeq ($(shell uname),Darwin)
	$(CXX) $(CXXFLAGS) -o $(LIBNAME) $(LIBFLAGS) dictTOP.C $(RATINC) $(TAINC) $(TALIBOBJ) $(ROOTLIBS) $(ROOTINC) $(RATLIB) -l$(MACRATLIB)
	ln -sf $(LIBNAME) $(LIBNAME:.dylib=.so) 
else ifeq ($(shell uname),Linux)
	$(CXX) $(CXXFLAGS) -fPIC -m64 -o libTrackAnalysis.so -shared dictTOP.C $(RATINC) $(TAINC) $(TALIBOBJ) $(ROOTLIBS) $(ROOTINC) $(RATLIB) -l$(LINRATLIB)
endif

TAbin: $(TAOBJ)
ifeq ($(shell uname),Darwin)
	$(CXX) $(CXXFLAGS) -o $(TABINNAME) $(TAINC) $(TALIB) $(TAOBJ) $(RATINC) $(ROOTINC) $(ROOTLIBS) $(RATLIB) -l$(MACRATLIB)
else ifeq ($(shell uname),Linux)
	$(CXX) $(CXXFLAGS) -o $(TABINNAME) $(TAINC) $(TALIB) $(TAOBJ) $(RATINC) $(ROOTINC) $(ROOTLIBS) $(RATLIB) -l$(LINRATLIB)
endif

clean:
	rm dictTOP.* 

distclean:
	rm dictTOP.* libTrackAnalysis.* trackanalysis
