ROOTLIBS:=$(shell root-config --libs)
ROOTINC:=-I$(shell root-config --incdir)
RATINC:=-I$(RATROOT)/include -I$(RATROOT)/src/stlplus
RATLIB:=-L$(RATROOT)/lib
TAINC:=-I./include
#For ROOTCINT
TOPOBJ:=include/TrackedOpticalPhoton.hpp
TOPLINKDEF:=include/LinkDef.h

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
CXXFLAGS:=-O2 -Wall

all: dictTOP.C libTA TAbin

dictTOP.C: $(TOPOBJ)
	rootcint dictTOP.C -c -p $(TOPOBJ) $(TOPLINKDEF)

libTA: dictTOP.C $(TALIBOBJ)
ifeq ($(shell uname),Darwin)
	$(CXX) $(CXXFLAGS) -o libTrackAnalysis.dylib -dynamiclib dictTOP.C $(RATINC) $(TAINC) $(TALIBOBJ) $(ROOTLIBS) $(ROOTINC) $(RATLIB) -l$(MACRATLIB)
	ln -sf libTrackAnalysis.dylib libTrackAnalysis.so
else ifeq ($(shell uname),Linux)
	$(CXX) $(CXXFLAGS) -o libTrackAnalysis.so -shared dictTOP.C $(RATINC) $(TAINC) $(TALIBOBJ) $(ROOTLIBS) $(ROOTINC) $(RATLIB) -l$(LINRATLIB)
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
