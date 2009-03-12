track_analysis_athena:
	icpc -openmp -o trackanalysis -Wall -lRATEvent_Linux-g++ -I`root-config --incdir` `root-config --libs` -I$(RATROOT)/src/stlplus -I$(RATROOT)/include -L$(RATROOT)/lib TrackAnalysisTreeOutputObject.cpp main.cpp
