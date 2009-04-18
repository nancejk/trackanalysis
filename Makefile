track_analysis_athena:
	icpc -UPRINT_TRACK_DEBUG -O2 -openmp -o trackanalysis -Wall -lRATEvent_Linux-g++ -I`root-config --incdir` `root-config --libs` -I$(RATROOT)/src/stlplus -I$(RATROOT)/include -L$(RATROOT)/lib TrackAnalysisTreeOutputObject.cpp main.cpp
track_analysis_athena_pt:
	icpc -DPRINT_TRACK_DEBUG -O2 -openmp -o trackanalysis -Wall -lRATEvent_Linux-g++ -I`root-config --incdir` `root-config --libs` -I$(RATROOT)/src/stlplus -I$(RATROOT)/include -L$(RATROOT)/lib TrackAnalysisTreeOutputObject.cpp main.cpp
track_analysis_athena_rd:
	icpc -DREFLECTION_DEBUG -O2 -openmp -o trackanalysis -Wall -lRATEvent_Linux-g++ -I`root-config --incdir` `root-config --libs` -I$(RATROOT)/src/stlplus -I$(RATROOT)/include -L$(RATROOT)/lib TrackAnalysisTreeOutputObject.cpp main.cpp

class_dict:
	rootcint dictTOP.C -c -p TrackedOpticalPhoton.hpp LinkDef.h
	g++ -dynamiclib -o libTrackedOpticalPhoton.dylib dictTOP.C TrackedOpticalPhoton.cpp -Wall -L/usr/local/root/Current/lib/root -I/usr/local/root/Current/include/root -lCore -lRIO -lTree -lcint

