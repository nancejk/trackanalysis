#include "TrackedOpticalPhoton.hpp" //For data record
#include <TTree.h> //For I/O
#include "RAT/DSReader.hh" //For reading in RAT files
#include "RAT/DS/Root.hh" //For reading in RAT data structures
#include "RAT/DS/MCTrackStep.hh"
#include <map>
#include <set>
#include <numeric>
#include <algorithm>
#include <iostream>

//A very useful map construct and its value type.
typedef std::map<unsigned,RAT::DS::MCTrack> IDtoTrackMap;
typedef std::pair<unsigned,RAT::DS::MCTrack> IDwithTrack;

TTree* GrowJoinedPhotonTree( RAT::DSReader& );
RAT::DS::MCTrack JoinMCTracks(std::vector<RAT::DS::MCTrack>);

struct TrackTimeOrdering
{
	bool operator() ( const RAT::DS::MCTrack& lhs, const RAT::DS::MCTrack& rhs )
	{
		return lhs.GetLastMCTrackStep()->GetGlobalTime() < rhs.GetLastMCTrackStep()->GetGlobalTime();
	}
};
