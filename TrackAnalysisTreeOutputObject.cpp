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
				"eventNo/i:fGenerationTime/F:fGenerationRadius:fGenerationEnergy:fPMTHitTime:fPMTHitEnergy:defHit/b:indefHit:reemitted:cerenkov:scintillation");

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
		std::queue<RAT::DS::MCTrack> tracks;
		//The way this object works is simple.  Each of its positions corresponds to a trackID.  If the trackID
		//in question is one that is known to have caused a photoelectron to be generated in a tube, then
		//known_hits[trackID] is true.  Otherwise it is false.  This provides a very fast and easy way to check
		//if a trackID is responsible for a hit in the monte carlo.
		std::vector<bool> known_hits( total_track_count, false ); 
		
		//Now we fill the queue with the optical photon tracks.
		for ( std::size_t mc_track_index = 0; mc_track_index < total_track_count; mc_track_index++ )
		{ 
			if ( theDSMC->GetMCTrack(mc_track_index)->GetParticleName() == "opticalphoton" ) 
			{ tracks.push( *theDSMC->GetMCTrack(mc_track_index) ); }
		}
		
		//Now we fix the vector up with the proper information about which tracks caused hits.
		for ( std::size_t mc_pmt_hit = 0; mc_pmt_hit < num_hits; mc_pmt_hit++ )
		{
			//Flip the bit in the TrackID'th slot.
			known_hits[ theDSMC->GetMCPMT(mc_pmt_hit)->GetMCPhoton(0)->GetTrackID() - 1 ].flip();
		}
		//Now print some status information.
		std::cout << "In current event: \n" 
				<< "\t" << tracks.size() << " photon tracks found.\n"
				<< "\t" << 100.0*static_cast<double>(tracks.size())/static_cast<double>(total_track_count) << " percent of total.\n"
		//Make sure that the number of confirmed hits as reported by MCPMTCount() and that
		//we've recorded in the vector<bool> are actually the same.  std::accumulate can do a good job
		//of this.
				<< "\t" << num_hits << " confirmed hits; " << std::accumulate(known_hits.begin(), known_hits.end(), 0) << " recorded."
				<< std::endl;
		
		//OK, we are now ready to process each individual track.  This is easy... just look at the top of the queue!
		while( tracks.empty() == false )
		{
			//First set the proper event index for this photon.
			thePhoton.eventNo = eventIndex;
			
			//tracks.front() object is the track we are working with!  It is in no particular spatial or temporal
			//order, but that is immaterial.
			RAT::DS::MCTrack curTrack = tracks.front();
			//Now check the trackID against the list of known hits.
			if ( known_hits[curTrack.GetTrackID() - 1] == true ) thePhoton.defHit = true;
						
			//Do the things we need to do at the beginning of the track.
			thePhoton.fGenerationTime = curTrack.GetMCTrackStep(0)->GetGlobalTime();
			thePhoton.fGenerationRadius = curTrack.GetMCTrackStep(0)->GetEndpoint().Mag();
			thePhoton.fGenerationEnergy = curTrack.GetMCTrackStep(0)->GetKE();
			
			//A boundary checking flag.  This gets used during the reflection checks.
			static bool LastStepEndedOnBoundary(false);			

			//Now we recurse into the track itself and do our dirty work.  Firstly, though,
			//if there is somehow only the zeroth track step, we need to skip this guy altogether.
			//Note that this is highly unlikely... but nonetheless, the code is unsafe without it.
			if ( curTrack.GetMCTrackStepCount() == 1 ) 
				{ tracks.pop(); continue; }
			
			//Start looping over the steps.
			for ( std::size_t step_index = 0; step_index < curTrack.GetMCTrackStepCount(); step_index++ )
			{
				//Get the current step and set the process variable.
				RAT::DS::MCTrackStep curStep = *curTrack.GetMCTrackStep(step_index);
				static std::string currentProcess = curStep.GetProcess();				

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
			tracks.pop();
		}
	}
	//Print the result and return it.
	theResultingTree->Print();
	return theResultingTree;
}


