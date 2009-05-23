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
	theTree->Print();
	std::cout << "Wrote " << _file.Sizeof() << 
				" bytes on " << outFile << std::endl;
	return EXIT_SUCCESS;
}
