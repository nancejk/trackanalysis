#include <iostream>
#include <vector>
#include <sys/stat.h>
#include "TrackAnalysisTreeOutputObject.h"
#include "TFile.h"
#include "RAT/DB.hh"
#include "RAT/DSReader.hh"

using namespace std;
using RAT::DSReader;
using RAT::DB;
using RAT::DBLinkPtr;

void PrintUsageAndDie();
bool DoesFileExist( std::string );

int main ( int argc, char * const argv[] )
{
	//Logic check at the beginning.
	if ( !(argc == 2 || argc == 3) )
	{
		PrintUsageAndDie();
	}
	
	//Get the file input name and output name parameters from the command line args.
	string filename( argv[1] );
	string outputname("");
	if ( argc == 3 ) outputname = argv[2];
	else outputname = "output.root";
	
	//Check to be sure that the input filename actually exists.  If not, exit.
	if ( DoesFileExist( filename ) == false ) 
	{
		std::cout << "No file with the name " << filename << " exists!" << std::endl;
		exit(1);
	}
	
	//Now get the RAT data structure from the file and be sure it contains entries before
	//passing it to the tree builder.	
	RAT::DSReader data( filename.c_str() );
	TFile theOutputFile( outputname.c_str(),"RECREATE" );
	assert ( data.GetTotal() != 0 );
	assert ( theOutputFile.IsZombie() != true );
		
	//Since we passed the assert (hopefully), we can build a tree.
	TTree* theTree = GrowJoinedPhotonTree(data);
	theOutputFile.Write();
	
	return 0;
}

void PrintUsageAndDie()
{
	std::cout << "Usage: RAT_Track_Extend [input filename] [output filename]" << std::endl;
	std::cout << "If no output filename is provided, the default filename output.root will be used." << std::endl;
	exit(1);
}

bool DoesFileExist( std::string theFilename )
{
	struct stat buffer;
	if ( stat( theFilename.c_str(), &buffer ) == 0 ) return 1;
	return 0;
}


/*
 int main (int argc, char * const argv[]) {
 //Just a quick check to make sure that the input logic is correct.
 if ( argc < 2 ) 
 {
 PrintUsageAndDie();
 }
 
 //Now get the filename we want to open and the filename we want to write out on.  If the filename isn't provided, 
 //we use the default name output.root.
 string filename( argv[1] );
 string outputname("");
 if ( argc == 3 ) outputname = argv[2];
 else outputname = "output.root";
 
 //Histogram information access section.  Load the RATDB file that contains the constants
 //and get a link to the correct table.
 std::cout << "Accessing histogram information..." << std::endl;
 DB* db = new RAT::DB;
 db->LoadAll("/Users/jkn/Documents/Code/xcode_projects/RATTracks");
 DBLinkPtr histdata = db->GetLink("TRACK_HIST_PARAMETERS");  
 
 //Now it's time to get the names of the histograms from the RATDB and feed it into the 
 //constructors for the OutputObject and the DataHarvester.
 vector<string> theHistNames = histdata->GetSArray("histogram_names");
 TrackAnalysisOutputObject theWriter( theHistNames );
 TrackAnalysisDataHarvester theWorker( theHistNames );
 
 //Quickly check to make sure the file we want to open really exists.
 if ( theWriter.DoesFileExist( filename ) == 0 ) 
 {
 std::cout << "No file with the name " << filename << " exists!" << std::endl;
 exit(1);
 }
 //And set up the output filename for the Writer.
 theWriter.SetOutputFileName(outputname);
 
 //Now set up the histograms in the outputobject with the RATDB data.
 for ( int ObjIndex = 0; ObjIndex < theHistNames.size(); ObjIndex++ )
 {
 std::cout << "\t" << "Registering histogram: " << theHistNames[ObjIndex] << std::endl;
 theWriter.SetUpHistogramByName(theHistNames[ObjIndex], 
 histdata->GetIArray("histogram_nbins")[ObjIndex],
 histdata->GetFArray("histogram_min")[ObjIndex],
 histdata->GetFArray("histogram_max")[ObjIndex] );
 }
 
 //OK, now open the file we've been talking about and point the data structure to NULL for
 //now.
 RAT::DSReader data( filename.c_str() );
 RAT::DS::Root *rDS = NULL;
 
 std::cout << "File " << filename << " opened.  Launching... " << std::endl;
 
 int EventIndex(0);
 int maxIndex = data.GetTotal();
 
 while ( rDS = data.NextEvent() )
 {
 std::cout << "Passing event number " << EventIndex << "/" << maxIndex << " to worker." << std::endl;
 theWorker.AnalyzeRATEvent(rDS);
 EventIndex++;
 }
 
 std::cout << "All data collected.  Collecting and writing out to " << outputname << "...";
 
 theWriter.CollectDataFromHarvester(theWorker);
 theWriter.WriteToFile();
 
 std::cout << " Success! " << std::endl;
 
 return 0;
 }
 */
