ROOTLIBS:=$(shell root-config --libs)
ROOTINC:=-I$(shell root-config --incdir)
RATINC:=-I$(RATROOT)/include
RATLIB:=-L$(RATROOT)/lib
TAINC:=-I./include
#For ROOTCINT
TOPOBJ:=include/TrackedOpticalPhoton.hpp
TOPLINKDEF:=include/LinkDef.h

#TRACKANALYSIS DEFS
TALIBOBJ:=src/TrackedOpticalPhoton.cpp src/MCTrackHelper.cpp
TAOBJ:=src/ta.cpp
TALIB:=-L. -lTrackAnalysis

#RATLIB DEFS
MACRATLIB:=RATEvent_Darwin-g++
LINRATLIB:=RATEvent_Linux-g++

all: dictTOP.C libTA TAbin

dictTOP.C: $(TOPOBJ)
	rootcint dictTOP.C -c -p $(TOPOBJ) $(TOPLINKDEF)

libTA: dictTOP.C $(TALIBOBJ)
ifeq ($(shell uname),Darwin)
	g++ -o libTrackAnalysis.dylib -dynamiclib dictTOP.C $(RATINC) $(TAINC) $(TALIBOBJ) $(ROOTLIBS) $(ROOTINC) $(RATLIB) -l$(MACRATLIB)
	ln -sf libTrackAnalysis.dylib libTrackAnalysis.so
else ifeq ($(shell uname),Linux)
	g++ -o libTrackAnalysis.so -shared dictTOP.C $(RATINC) $(TAINC) $(TALIBOBJ) $(ROOTLIBS) $(ROOTINC) $(RATLIB) -l$(LINRATLIB)
endif

TAbin: $(TAOBJ)
	

clean:
	rm dictTOP.*
distclean:
	rm dictTOP.* libTrackAnalysis.*
