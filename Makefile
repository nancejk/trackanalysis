track_analysis_athena:
	icpc -UPRINT_TRACK_DEBUG -O2 -openmp -o trackanalysis -Wall -lRATEvent_Linux-g++ -I`root-config --incdir` `root-config --libs` -I$(RATROOT)/src/stlplus -I$(RATROOT)/include -L$(RATROOT)/lib TrackAnalysisTreeOutputObject.cpp main.cpp
track_analysis_athena_pt:
	icpc -DPRINT_TRACK_DEBUG -O2 -openmp -o trackanalysis -Wall -lRATEvent_Linux-g++ -I`root-config --incdir` `root-config --libs` -I$(RATROOT)/src/stlplus -I$(RATROOT)/include -L$(RATROOT)/lib TrackAnalysisTreeOutputObject.cpp main.cpp
