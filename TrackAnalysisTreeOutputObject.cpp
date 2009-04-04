/*
 *  TrackAnalysisTreeOutputObject.cpp
 *  RAT_track_extend
 *
 *  Created by Jared Nance on 1/5/09.
 *  Copyright 2009 University of Washington. All rights reserved.
 *
 */

#include "TrackAnalysisTreeOutputObject.h"
#include <iostream>

using std::string;

void ClearTrackedPhoton(TrackedOpticalPhoton& thePhoton)
{
	thePhoton.eventNo = 0;
	thePhoton.parentID = 0;
	thePhoton.fGenerationTime = 0;
	thePhoton.fGenerationRadius = 0;
	thePhoton.fGenerationEnergy = 0;
	thePhoton.fPMTHitTime = 0;
	thePhoton.fPMTHitEnergy = 0;
	thePhoton.defHit = false;
	thePhoton.indefHit = false;
	thePhoton.reemitted = false;
	thePhoton.cerenkov = false;	
	thePhoton.scintillation = false;
}

TTree* GrowPhotonTree( RAT::DSReader& theDS )
{
	//Now we build the fundamental blocks of our operation.  The TrackedOpticalPhoton is static to avoid
	//building many copies of the object on the stack, which is unnecessary.  We reset the photon at the
	//end of each track for this reason.
	RAT::DS::Root* theDSEvent = NULL;
	RAT::DS::MC* theDSMC = NULL;
	static TrackedOpticalPhoton thePhoton;
	TTree* theResultingTree = new TTree("T","Data From Tracked Optical Photons");
	TBranch* theBranch = theResultingTree->Branch(
				"TrackedOpticalPhotons",
			  	&thePhoton,
				"eventNo/i:parentID:fGenerationTime/F:fGenerationRadius:fGenerationEnergy:fPMTHitTime:fPMTHitEnergy:defHit/b:indefHit:reemitted:cerenkov:scintillation");

	for ( int eventIndex = 0; eventIndex < theDS.GetTotal(); eventIndex++  )
	{
		//Move to the next event.
		theDSEvent = theDS.GetEvent(eventIndex);
		theDSMC = theDSEvent->GetMC();
		//Now for future reference, the number of hits:
		std::size_t num_hits = theDSMC->GetMCPMTCount();
		std::size_t total_track_count = theDSMC->GetMCTrackCount();

		std::cout << "Processing event number " << eventIndex << " of " << theDS.GetTotal() << std::endl;
		//At this point we should already know that the event is good (i.e. actually contains track data).
		//Now instead of using the TrackNav and TrackCursor, we build two objects. One is a queue containing
		//MCTracks, and the other is a boolean vector that will contain information about the 'real' hits.
		std::deque<RAT::DS::MCTrack> tracks;
		//The waythis object works is simple.  Each of its positions corresponds to a trackID.  If the trackID
		//in question is one that is known to have caused a photoelectron to be generated in a tube, then
		//known_hits[trackID] is true.  Otherwise it is false.  This provides a very fast and easy way to check
		//if a trackID is responsible for a hit in the monte carlo.
		std::vector<bool> known_hits( total_track_count, false ); 
		
		//Now we fill the queue with the optical photon tracks.
		for ( std::size_t mc_track_index = 0; mc_track_index < total_track_count; mc_track_index++ )
		{ 
			if ( theDSMC->GetMCTrack(mc_track_index)->GetParticleName() == "opticalphoton" ) 
			{ tracks.push_back( *theDSMC->GetMCTrack(mc_track_index) ); }
		}
		
		//Now we fix the vector up with the proper information about which tracks caused hits.
		for ( std::size_t mc_pmt_hit = 0; mc_pmt_hit < num_hits; mc_pmt_hit++ )
		{
			//Iterate through all MCPhotons, flipping the TrackID'th bit in the vector<bool>
			//to indicate a hit.
			for ( std::size_t mc_phot = 0; mc_phot < theDSMC->GetMCPMT(mc_pmt_hit)->GetMCPhotonCount(); mc_phot++ )
			{	
				known_hits[ theDSMC->GetMCPMT(mc_pmt_hit)->GetMCPhoton(mc_phot)->GetTrackID() - 1 ].flip();
			}
		}
		//Now print some status information.
		std::cout << "In current event: \n" 
				<< "\t" << tracks.size() << " optical photon tracks found\n"
				<< "\t" << 100.0*static_cast<double>(tracks.size())/static_cast<double>(total_track_count) << " percent of total.\n"
		//Make sure that the number of confirmed hits as reported by MCPMTCount() and that
		//we've recorded in the vector<bool> are actually the same.  std::accumulate can do a good job
		//of this.
				<< "\t" << num_hits << " hit PMTs; " << std::accumulate(known_hits.begin(), known_hits.end(), 0) << " individual hits recorded."
				<< std::endl;
		
		//OK, we are now ready to process each individual track.  This is easy... just look at the top of the queue!
		while( tracks.empty() == false )
		{
			//First set the proper event index for this photon.
			thePhoton.eventNo = eventIndex;
			
			//tracks.front() object is the track we are working with!  It is in no particular spatial or temporal
			//order, but that is immaterial.
			RAT::DS::MCTrack curTrack = tracks.front();
			thePhoton.parentID = curTrack.GetParentID();
			//Now check the trackID against the list of known hits.
			if ( known_hits[curTrack.GetTrackID() - 1] == true ) thePhoton.defHit = true;
						
			//Do the things we need to do at the beginning of the track.
			thePhoton.fGenerationTime = curTrack.GetMCTrackStep(0)->GetGlobalTime();
			thePhoton.fGenerationRadius = curTrack.GetMCTrackStep(0)->GetEndpoint().Mag();
			thePhoton.fGenerationEnergy = curTrack.GetMCTrackStep(0)->GetKE();
			
			//A boundary checking flag.  This gets used during the reflection checks.
			bool LastStepEndedOnBoundary(false);			
			std::string currentProcess("");

			//Now we recurse into the track itself and do our dirty work.  Firstly, though,
			//if there is somehow only the zeroth track step, we need to skip this guy altogether.
			//Note that this is highly unlikely... but nonetheless, the code is unsafe without it.
			if ( curTrack.GetMCTrackStepCount() == 1 ) 
				{ tracks.pop_front(); continue; }
			
			//Start looping over the steps.
			for ( std::size_t step_index = 0; step_index < curTrack.GetMCTrackStepCount(); step_index++ )
			{
				//Get the current step and set the process variable.
				RAT::DS::MCTrackStep curStep = *curTrack.GetMCTrackStep(step_index);
				currentProcess = curStep.GetProcess();

				//Checks to see if the photon has been reemitted.  If so, mark it.  This doesn't
				//actually do anything if the photon has already been marked as such.  This 
				//conditional is short-circuited, so the second statement won't be checked unless
				//the first is true.
				if ( ( currentProcess == "Reemission" ) && ( thePhoton.reemitted == false ) )
					{ thePhoton.reemitted = true; }
				
				if ( ( currentProcess == "Scintillation" ) && ( thePhoton.scintillation == false ) )
					{ thePhoton.scintillation = true; }
				
				//Checks to see if the photon is Cerenkov radiation.
				if ( ( currentProcess == "Cerenkov" ) && ( thePhoton.cerenkov == false ) )
					{ thePhoton.cerenkov = true; }
				
				//This will check if the current step is involved in a process that can
				//lead to a hit being recorded in the monte carlo.  It is no guarantee that
				//it is actually a hit (a definite hit), as that is determined probabilistically at
				//(simulation) runtime.  Therefore this is called an indefinite hit.  Not all 
				//indefinite hits are definite hits, but all definite hits are indefinite hits.
				if ( currentProcess.find("G4FastSimulationManagerProcess") != string::npos ) 
					{ thePhoton.fPMTHitTime = curStep.GetGlobalTime(); thePhoton.fPMTHitEnergy = curStep.GetKE(); thePhoton.indefHit = true; }

				//If this step ended on a boundary, then the StepStatus will mark it as fGeomBoundary.
				//During the next step, we can check if this flag was set, and if so, do reflection
				//checking.  If it didn't, but the flag is set from the previous step, unset it.
				if ( curStep.GetStepStatus() == "GeomBoundary") LastStepEndedOnBoundary = true;
				else if ( LastStepEndedOnBoundary ) LastStepEndedOnBoundary = false;
			}
			
			//We've done all of the necessary steps, so on to filling the tree and
			//finishing the iteration.
			theResultingTree->Fill();
			ClearTrackedPhoton(thePhoton);
			tracks.pop_front();
		}
	}
	//Print the result and return it.
	theResultingTree->Print();
	return theResultingTree;
}

//Returns the time-ordering of two given tracks based on their endpoint; if 
//track A ended before track B (in terms of time), then track A must have 
//occurred before track B, and therefore belongs before it.
struct TrackTimeOrdering
{
	bool operator() ( const RAT::DS::MCTrack& lhs, const RAT::DS::MCTrack& rhs )
	{
		return lhs.GetLastMCTrackStep()->GetGlobalTime() < rhs.GetLastMCTrackStep()->GetGlobalTime();
	}
};

RAT::DS::MCTrack JoinMCTracks(std::vector<RAT::DS::MCTrack> theTrackList)
{
	//Typedef the time ordered set of tracks.
	typedef std::set<RAT::DS::MCTrack,TrackTimeOrdering> TOTrackSet;
	//Build a time ordered set of tracks.
	TOTrackSet TOTracks( theTrackList.begin(), theTrackList.end() );
	
	//Now create a new track that we will fill with all of the old steps.  We
	//can use a copy of the first track to get started.
	RAT::DS::MCTrack theJoinedTrack = *TOTracks.begin();
	
	//Now just iterate over all members of the set and fill the new track with
	//the old steps.
	typedef TOTrackSet::iterator setIt; 
	//Get an iterator to the beginning of the TOTracks and move it forward one
	//step past the very first.
	setIt TOTracksIterator = TOTracks.begin();
	TOTracksIterator++;
	
	while ( TOTracksIterator != TOTracks.end() )
	{
		for ( unsigned i = 0; i < TOTracksIterator->GetMCTrackStepCount(); i++ )
		{
			*theJoinedTrack.AddNewMCTrackStep() = *TOTracksIterator->GetMCTrackStep(i); 
		}
		TOTracksIterator++;
	}
	
	return theJoinedTrack;
}

TTree* GrowJoinedPhotonTree( RAT::DSReader& theDS )
{
	//Now we build the fundamental blocks of our operation.  The TrackedOpticalPhoton is static to avoid
	//building many copies of the object on the stack, which is unnecessary.  We reset the photon at the
	//end of each track for this reason.
	RAT::DS::Root* theDSEvent = NULL;
	RAT::DS::MC* theDSMC = NULL;
	static TrackedOpticalPhoton thePhoton;
	TTree* theResultingTree = new TTree("T","Data From Tracked Optical Photons");
	TBranch* theBranch = theResultingTree->Branch(
												  "TrackedOpticalPhotons",
												  &thePhoton,
												  "eventNo/i:parentID:fGenerationTime/F:fGenerationRadius:fGenerationEnergy:fPMTHitTime:fPMTHitEnergy:defHit/b:indefHit:reemitted:cerenkov:scintillation");
	//All we are going to do, for starters, is get the tracks, print out the 
	//steps, join the tracks, and print them out again.  Easy stuff.  This will
	//also give an easy way to track where the code is going wrong (or right).
	for ( int eventIndex = 0; eventIndex < theDS.GetTotal(); eventIndex++  )
	{
		//Move to the next event.
		theDSEvent = theDS.GetEvent(eventIndex);
		theDSMC = theDSEvent->GetMC();

		//Spit out Event Info...
		std::cout << "Analyzing event " << eventIndex << std::endl;
		
		//Our container for the track themselves.  I am using a deque instead
		//of a vector because the vast majority of elements will get pushed
		//onto the front and back, and this structure is way more efficient
		//doing that.
		typedef std::deque<RAT::DS::MCTrack> trackdeque;
		trackdeque tracks;
		
		//Now fill the deque with the tracks.
		for ( std::size_t track = 0; track < theDSMC->GetMCTrackCount(); track++ )
		{
			//Check if the track is an optical photon.
			if ( theDSMC->GetMCTrack(track)->GetParticleName() == "opticalphoton" )
			{
				//Push a reference to it onto the deque.
				tracks.push_back(*theDSMC->GetMCTrack(track));
			}
		}
		
		//Now that our deque is full of tracks, let's iterate over them and
		//print out their information.  This is, of course, pre-joining.
		trackdeque::iterator track_it = tracks.begin();
		while ( track_it != tracks.end() )
		{
			std::cout << "Track ID " << track_it->GetTrackID() << "->Child of track ID " << track_it->GetParentID() << "\n";
			for ( std::size_t stepcount = 0; stepcount < track_it->GetMCTrackStepCount(); stepcount++ )
			{
				std::cout << "\t Step " << stepcount << "->" << track_it->GetMCTrackStep(stepcount)->GetProcess() << "\n" 
					  << "\t\t X:" << track_it->GetMCTrackStep(stepcount)->GetEndpoint().X() << "\n"
					  << "\t\t Y:" << track_it->GetMCTrackStep(stepcount)->GetEndpoint().Y() << "\n"
					  << "\t\t Z:" << track_it->GetMCTrackStep(stepcount)->GetEndpoint().Z() << "\n"
					  << "\t\t T:" << track_it->GetMCTrackStep(stepcount)->GetGlobalTime() <<   "\n"
					  << "\t\t KE:" << track_it->GetMCTrackStep(stepcount)->GetKE() << std::endl;
			}
			//Remember to increment the iterator!
			track_it++;
		}
		
		//OK, now the hard work.  We need to join these damn tracks.  This map will
		//map between trackID and a std::size_t which corresponds to the position
		//of the track in the trackdeque.
		typedef std::map<unsigned,size_t> IDtoLocMap;
		typedef std::pair<unsigned,std::size_t> IDLocPair;
		IDtoLocMap track_position;
		for ( std::size_t dequepos = 0; dequepos < tracks.size(); dequepos++ )
		{
			//Implicitly construct a pair with the proper tags and insert it into
			//the map.
			track_position.insert( IDLocPair(tracks[dequepos].GetTrackID(),dequepos ) );
		}
		
		//Now we can look up a track by its ID.  So start looking through the tracks
		//for tracks that have parents, and see if they are in the map.  If they
		//are, that means that they were born from an optical photon, so we care.
		//Start from the back and go towards the front.
		trackdeque::reverse_iterator track_rit = tracks.rbegin();
		while ( track_rit != tracks.rend() )
		{
			//Search for the ID of the parent track in the map.  If it is found,
			//we have work to do.
			IDtoLocMap::iterator mall_guard = track_position.find( track_rit->GetParentID() );
			if ( mall_guard != track_position.end() )
			{
				//Create a vector of the tracks that are related, and start looking
				//to see if they have parents of their own.
				std::vector<RAT::DS::MCTrack> estranged_tracks;
				estranged_tracks.push_back(tracks[mall_guard->second]);
				estranged_tracks.push_back(*track_rit);
				
				mall_guard = track_position.find( tracks[mall_guard->second].GetParentID() );
				while ( mall_guard != track_position.end() )
				{
					estranged_tracks.push_back(tracks[mall_guard->second]);
					mall_guard = track_position.find( tracks[mall_guard->second].GetParentID() );
				}
				
				//Now we should have all of the parents.  Assemble them via
				//the JoinMCTracks function.  For now, just stick it on the end
				//of the tracks we've already made.
				tracks.push_back(JoinMCTracks(estranged_tracks));
			}
			track_rit++;
		}
		
		//OK, now print them out again.
		//Now that our deque is full of tracks, let's iterate over them and
		//print out their information.  This is, of course, pre-joining.
		std::cout << "----------CORRECTED TRACKS FOLLOW----------" << std::endl;
		while ( track_it != tracks.end() )
		{
			std::cout << "Track ID " << track_it->GetTrackID() << "->Child of track ID " << track_it->GetParentID() << "\n";
			for ( std::size_t stepcount = 0; stepcount < track_it->GetMCTrackStepCount(); stepcount++ )
			{
				std::cout << "\t Step " << stepcount << "->" << track_it->GetMCTrackStep(stepcount)->GetProcess() << "\n" 
				<< "\t\t X:" << track_it->GetMCTrackStep(stepcount)->GetEndpoint().X() << "\n"
				<< "\t\t Y:" << track_it->GetMCTrackStep(stepcount)->GetEndpoint().Y() << "\n"
				<< "\t\t Z:" << track_it->GetMCTrackStep(stepcount)->GetEndpoint().Z() << "\n"
				<< "\t\t T:" << track_it->GetMCTrackStep(stepcount)->GetGlobalTime() <<   "\n"
				<< "\t\t KE:" << track_it->GetMCTrackStep(stepcount)->GetKE() << std::endl;
			}
			//Remember to increment the iterator!
			track_it++;
		}
		
	}
	
		
	//Now just return the tree we built.
	return theResultingTree;
}
	

