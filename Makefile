RATROOT := /Users/nancejk/Code/svn_repos/sasquatch/dev/rat

track_analysis_athena:
	icpc -UPRINT_TRACK_DEBUG -O2 -openmp -o trackanalysis -Wall -lRATEvent_Linux-g++ -I`root-config --incdir` `root-config --libs` -I$(RATROOT)/src/stlplus -I$(RATROOT)/include -L$(RATROOT)/lib TrackAnalysisTreeOutputObject.cpp main.cpp
track_analysis_athena_pt:
	icpc -DPRINT_TRACK_DEBUG -O2 -openmp -o trackanalysis -Wall -lRATEvent_Linux-g++ -I`root-config --incdir` `root-config --libs` -I$(RATROOT)/src/stlplus -I$(RATROOT)/include -L$(RATROOT)/lib TrackAnalysisTreeOutputObject.cpp main.cpp
track_analysis_athena_rd:
	icpc -DREFLECTION_DEBUG -O2 -openmp -o trackanalysis -Wall -lRATEvent_Linux-g++ -I`root-config --incdir` `root-config --libs` -I$(RATROOT)/src/stlplus -I$(RATROOT)/include -L$(RATROOT)/lib TrackAnalysisTreeOutputObject.cpp main.cpp

all: class_dict test

class_dict:
	rootcint dictTOP.C -c -p TrackedOpticalPhoton.hpp LinkDef.h
	g++ -dynamiclib -o libTrackAnalysis.dylib dictTOP.C TrackedOpticalPhoton.cpp -Wall -L/usr/local/root/Current/lib/root -I/usr/local/root/Current/include/root -lCore -lRIO -lTree -lcint -lPhysics
	ln -fs libTrackAnalysis.dylib libTrackAnalysis.so

routine:
	g++ -o trackanalysis -Wall -L. -lTrackAnalysis dictTOP.C TrackedOpticalPhoton.cpp -L$(ROOTSYS)/lib/root -I$(ROOTSYS)/include/root -lCore -lRIO -lTree -lcint -lPhysics

test:
	sh /Users/nancejk/Code/svn_repos/sasquatch/dev/rat/env.sh
	g++ -o toptest -Wall -I. -L. -lTrackAnalysis test.cpp MCTrackHelper.cpp -L$(ROOTSYS)/lib/root -I$(ROOTSYS)/include/root -lCore -lRIO -lTree -lcint -lPhysics -I$(RATROOT)/src/stlplus -I$(RATROOT)/include -L$(RATROOT)/lib -lRATEvent_Darwin-g++

unmake:
	rm dictTOP.C dictTOP.h
	rm toptest
	rm libTrackAnalysis.so libTrackAnalysis.dylib
