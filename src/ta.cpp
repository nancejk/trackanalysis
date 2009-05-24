#include "TrackedOpticalPhoton.hpp"
#include "MCTrackHelper.hpp"
#include <iostream>
#include "TFile.h"
#include <cstdlib>
#include <string>

//Print the correct usage syntax and exit() with return code 1.
void UsageAndDie();

//The main routine.  Loads the data file, creates the output file, and 
//calls the function that will process the data.
int main(int argc, const char* argv[])
{
	//If the incorrect number of arguments is passed, output usage information
	//and die.
	if ( argc != 3 ) UsageAndDie();
	
	//Otherwise...
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

//Print appropriate usage information and die.
void UsageAndDie()
{
	std::cout << "trackanalysis\n";
	std::cout << "Photon process accounting post-processor for RAT data.\n";
	std::cout << "Written by Jared Nance (University of Washington)\n";
	std::cout << "\t nancejk@phys.washington.edu\n\n";
	std::cout << "Usage:\n";
	std::cout << "\t" << "trackanalysis <input file name> <output file name>";
	exit(1);
}