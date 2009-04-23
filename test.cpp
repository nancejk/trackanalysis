#include "TrackedOpticalPhoton.hpp"
#include "MCTrackHelper.hpp"
#include <iostream>
#include "TFile.h"
#include <cstdlib>
int main()
{
	TFile _file("tatest.root","RECREATE");
	RAT::DSReader data("testdata.root");
	TTree* theTree = GrowJoinedPhotonTree(data);
	
	_file.Write();

	return EXIT_SUCCESS;
}
