#include "TrackedOpticalPhoton.hpp"
#include "MCTrackHelper.hpp"
#include <iostream>
#include "TFile.h"
#include <cstdlib>
#include <string>
int main(int argc, const char* argv[])
{
	std::string inFile(argv[1]);
	std::string outFile(argv[2]);
	TFile _file(outFile.c_str(),"RECREATE");
	RAT::DSReader data(inFile.c_str());
	TTree* theTree = GrowJoinedPhotonTree(data);
	
	_file.Write();

	return EXIT_SUCCESS;
}
