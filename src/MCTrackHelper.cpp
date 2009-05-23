#include "MCTrackHelper.hpp"

TTree* GrowJoinedPhotonTree( RAT::DSReader& theDS )
{
	//Now we build the fundamental blocks of our operation.  The TrackedOpticalPhoton is static to avoid
	//building many copies of the object on the stack, which is unnecessary.  We reset the photon at the
	//end of each track for this reason.
	RAT::DS::Root* theDSEvent = NULL;
	RAT::DS::MC* theDSMC = NULL;
	static TrackedOpticalPhoton thePhoton;
	TTree* theResultingTree = new TTree("T",
										"Data From Tracked Optical Photons");

//For ROOT compatibility.  For versions 5.22 and up, it's legal to omit the
//name of the class in this call.  Before then, we need to call it by name.
#ifdef __ROOTVERLT522
	TBranch* theBranch = theResultingTree->Branch("TrackedOpticalPhotons",
										"TrackedOpticalPhoton",&thePhoton);
#else 
	TBranch* theBranch = theResultingTree->Branch("TrackedOpticalPhotons",
											&thePhoton);
#endif
	//This is just to keep the compiler from complaining about the use of the
	//TBranch.
	theBranch->UpdateAddress();
	
	//All we are going to do, for starters, is get the tracks, print out the 
	//steps, join the tracks, and print them out again.  Easy stuff.  This will
	//also give an easy way to track where the code is going wrong (or right).
	for ( int eventIndex = 0; eventIndex < theDS.GetTotal(); eventIndex++  )
	{
		//Move to the next event.
		theDSEvent = theDS.GetEvent(eventIndex);
		theDSMC = theDSEvent->GetMC();
#ifndef CLUSTER_RUN
		//Spit out Event Info...
		std::cout << std::endl << "Analyzing event " << eventIndex << std::endl;
#endif		
		//The container will be a map, where the key is the trackID and the 
		//value is the track itself.  This structure is _very_ useful for 
		//track reconstruction, where you want to be able to get a track by
		//referring to its id.
		IDtoTrackMap tracks;
		
		//We also want to have a container that can tell us if a given track
		//caused a hit in the simulation.  We use a vector<bool>, where the
		//index is equal to the trackID.
		std::vector<bool> hit_list( theDSMC->GetMCTrackCount(), false );
		
		//Now fill the map with the tracks.
		for ( int track = 0; track < theDSMC->GetMCTrackCount(); track++ )
		{
			//Check if the track is an optical photon.
			if ( theDSMC->GetMCTrack(track)->GetParticleName() == "opticalphoton" )
			{
				//Grab the track out of the monte carlo and push it into the map
				//with its ID.
				RAT::DS::MCTrack newTrack = *theDSMC->GetMCTrack(track);
				tracks.insert( IDwithTrack(newTrack.GetTrackID(), newTrack) );
			}
		}
		
		//And now for every hit in the monte carlo, flip the appropriate bit.
		for ( int mc_pmt_hit = 0; mc_pmt_hit < theDSMC->GetMCPMTCount(); mc_pmt_hit++ )
		{
			//Iterate through all MCPhotons, flipping the TrackID'th bit in the vector<bool>
			//to indicate a hit.
			for ( int mc_phot = 0; mc_phot < theDSMC->GetMCPMT(mc_pmt_hit)->GetMCPhotonCount(); mc_phot++ )
			{	
				hit_list[ theDSMC->GetMCPMT(mc_pmt_hit)->GetMCPhoton(mc_phot)->GetTrackID() - 1 ].flip();
			}
		}
#ifndef CLUSTER_RUN
#ifdef PRINT_TRACK_DEBUG	
		//Now that our map is full of tracks, let's iterate over them and
		//print out their information.  This is, of course, pre-joining.
		IDtoTrackMap::iterator track_it = tracks.begin();
		while ( track_it != tracks.end() )
		{
			std::cout << "Track ID " << track_it->first << "->Child of track ID " << track_it->second.GetParentID() << "\n";
			for ( std::size_t stepcount = 0; stepcount < track_it->second.GetMCTrackStepCount(); stepcount++ )
			{
				std::cout << "\t Step " << stepcount << "->" << track_it->second.GetMCTrackStep(stepcount)->GetProcess() << "\n" 
					  << "\t\t X:" << track_it->second.GetMCTrackStep(stepcount)->GetEndpoint().X() << "\n"
					  << "\t\t Y:" << track_it->second.GetMCTrackStep(stepcount)->GetEndpoint().Y() << "\n"
					  << "\t\t Z:" << track_it->second.GetMCTrackStep(stepcount)->GetEndpoint().Z() << "\n"
					  << "\t\t T:" << track_it->second.GetMCTrackStep(stepcount)->GetGlobalTime() <<   "\n"
					  << "\t\t KE:" << track_it->second.GetMCTrackStep(stepcount)->GetKE() << std::endl;
			}
			//Remember to increment the iterator!
			track_it++;
		}
#endif
#endif		
		//OK, now the hard work.  We need to join these damn tracks.  Use a 
		//reverse iterator to move back through the tracks and look for child-
		//parent relationships.  This will simplify the logic of removing tracks 
		//from the map that 'belong' to some parent.
		IDtoTrackMap::reverse_iterator track_rit = tracks.rbegin();
		while ( track_rit != tracks.rend() )
		{
			//Search for the ID of the parent track in the map.  If it is found,
			//we have work to do.
			IDtoTrackMap::iterator mall_guard = tracks.find( track_rit->second.GetParentID() );
			if ( mall_guard != tracks.end() )
			{
				//Create a vector of the tracks that are related, and start looking
				//to see if they have parents of their own.
				std::vector<RAT::DS::MCTrack> estranged_tracks;
				estranged_tracks.push_back( mall_guard->second );
				estranged_tracks.push_back( track_rit->second );
				
				mall_guard = tracks.find( mall_guard->second.GetParentID() );
				while ( mall_guard != tracks.end() )
				{
					estranged_tracks.push_back( mall_guard->second );
					mall_guard = tracks.find( mall_guard->second.GetParentID() );
				}
				//Now here's the trick.  The original tracks need to be 
				//__removed__ from the track listing.  Otherwise, when we step
				//to the next track, we are going to have issues due to the 
				//fact that they are tracks in the chain.  Of course, this is
				//fine, because we have retained all of the information they
				//had anyhow.  While we are doing this, we want to check to see
				//if any of the tracks caused hits in the monte carlo.  If they
				//did, we need to flip the new bit in the hitlist.
				std::vector<RAT::DS::MCTrack>::iterator est_it = estranged_tracks.begin();
				bool ChildHit(false);
				while ( est_it != estranged_tracks.end() )
				{
					unsigned theChildID = est_it->GetTrackID();
					if ( hit_list[theChildID - 1] ) 
					{
						//Mark the flag and unset the hitlist for the child.
						ChildHit = true;
						hit_list[theChildID - 1].flip();
					}
					tracks.erase( tracks.find(theChildID)->first );
					est_it++;
				}
				//Now we should have all of the parents.  Assemble them via
				//the JoinMCTracks function.  For now, just stick it on the end
				//of the tracks we've already made.
				
				RAT::DS::MCTrack joined = JoinMCTracks(estranged_tracks);
				tracks.insert( IDwithTrack(joined.GetTrackID(),joined) );
				
				//If the ChildHit bit is flipped, make sure that the trackID of
				//the new joined track gets set.
				if ( ChildHit ) hit_list[ joined.GetTrackID() - 1 ].flip();

				//Now the iterator needs to be reset, because we've deleted elements
				//from the map, which _should_ invalidate the pointer, but it 
				//doesn't because find() returns a forward iterator.
				track_rit = tracks.rbegin();
			}
			else track_rit++;
		}
#ifndef CLUSTER_RUN		
#ifdef PRINT_TRACK_DEBUG
		//Post-joined info.
		track_it = tracks.begin();
		std::cout << "----------CORRECTED TRACKS FOLLOW----------" << std::endl;
		while ( track_it != tracks.end() )
		{
			std::cout << "Track ID " << track_it->second.GetTrackID() << "->Child of track ID " << track_it->second.GetParentID() << "\n";
			for ( std::size_t stepcount = 0; stepcount < track_it->second.GetMCTrackStepCount(); stepcount++ )
			{
				std::cout << "\t Step " << stepcount << "->" << track_it->second.GetMCTrackStep(stepcount)->GetProcess() << "\n" 
				<< "\t\t X:" << track_it->second.GetMCTrackStep(stepcount)->GetEndpoint().X() << "\n"
				<< "\t\t Y:" << track_it->second.GetMCTrackStep(stepcount)->GetEndpoint().Y() << "\n"
				<< "\t\t Z:" << track_it->second.GetMCTrackStep(stepcount)->GetEndpoint().Z() << "\n"
				<< "\t\t T:" << track_it->second.GetMCTrackStep(stepcount)->GetGlobalTime() <<   "\n"
				<< "\t\t KE:" << track_it->second.GetMCTrackStep(stepcount)->GetKE() << std::endl;
			}
			//Remember to increment the iterator!
			track_it++;
		}
#endif
#endif
		//Now the tracks that were split by reemissions etc have been rejoined.  We
		//are therefore free to do the analysis on them.  Using the map iterator to
		//jump through the tracks should be easiest and fastest
#ifndef PRINT_TRACK_DEBUG
		//If the debug flag wasn't set, this iterator won't be in memory.
        //So we declare it here.
        IDtoTrackMap::iterator track_it;
#endif
		std::cout << tracks.size() << " reconstructed tracks found." << std::endl;
		track_it = tracks.begin();
		
		//A counter so we can build in a progress indicator.
		std::size_t track_index(1);
		while ( track_it != tracks.end() )
		{
#ifndef CLUSTER_RUN
			std::cout << track_index << " of " << tracks.size() << " processed.\r" << std::flush;
#endif
			//First set the proper event index for this photon.
			thePhoton.fEventNo = eventIndex;
			
			//tracks.front() object is the track we are working with!  It is in no particular spatial or temporal
			//order, but that is immaterial.
			RAT::DS::MCTrack curTrack = track_it->second;
			thePhoton.fParentID = curTrack.GetParentID();
			//Set the birthplace for this track.
			thePhoton.fBirthX = curTrack.GetMCTrackStep(0)->GetEndpoint().X();
			thePhoton.fBirthY = curTrack.GetMCTrackStep(0)->GetEndpoint().Y();
			thePhoton.fBirthZ = curTrack.GetMCTrackStep(0)->GetEndpoint().Z();
			thePhoton.totalLength = curTrack.GetLength();
			//Now check the trackID against the list of known hits.
			if ( hit_list[curTrack.GetTrackID() - 1] == true ) thePhoton.MarkDefiniteHit();
		
			//Boundary and process checking.
			static std::string currentProcess("");
			static bool LastStepEndedOnBoundary(false);
			
			//Since this is the beginning, we can figure out what process generated this track.
			std::string parentProcess = curTrack.GetMCTrackStep(0)->GetProcess();
			if ( parentProcess == "Scintillation" ) thePhoton.isScintillation = true;
			else if ( parentProcess == "Cerenkov" ) thePhoton.isCerenkov = true;
			
			//Now onto the step checks.
			for ( int step_index = 0; step_index < curTrack.GetMCTrackStepCount(); step_index++ )
			{
				//Get the current step and set the process variable.
				RAT::DS::MCTrackStep curStep = *curTrack.GetMCTrackStep(step_index);
				currentProcess = curStep.GetProcess();
				
				//If the last step ended on a boundary...
				if ( LastStepEndedOnBoundary && step_index != 0 )
				{
					//This will track the totally internally reflected photons.
					//To actually capture the step that reflects, we need to get
					//the last photon back!
					if ( curStep.GetEndVolume() == curTrack.GetMCTrackStep(step_index-1)->GetVolume() ) 
					{
						RAT::DS::MCTrackStep temp_track_step = *curTrack.GetMCTrackStep(step_index-1);
						thePhoton.AddReflection( temp_track_step.GetEndpoint().X(),
									 temp_track_step.GetEndpoint().Y(),
									 temp_track_step.GetEndpoint().Z(),
									 temp_track_step.GetGlobalTime()  );
					}
				}				
		
				//This will check if the current step is involved in a process that can
				//lead to a hit being recorded in the monte carlo.  It is no guarantee that
				//it is actually a hit (a definite hit), as that is determined probabilistically at
				//(simulation) runtime.  Therefore this is called an indefinite hit.  Not all 
				//indefinite hits are definite hits, but all definite hits are indefinite hits.
				if ( currentProcess.find("G4FastSimulationManagerProcess") != std::string::npos ) 
				{
					thePhoton.AddPMTHit( curStep.GetEndpoint().X(),
							     curStep.GetEndpoint().Y(),
							     curStep.GetEndpoint().Z(),
							     curStep.GetGlobalTime() );
				}
				
				//This is the process accounting step.  If the photon was reemitted, mark it.
				if ( currentProcess == "Reemission" )
				{
					//If we haven't already marked this photon as being reemitted, mark it.
					if ( thePhoton.isReemitted == false ) thePhoton.isReemitted = true;
					thePhoton.ReemissionCount++;					
				}
				
				//If this step ended on a boundary, then the StepStatus will mark it as fGeomBoundary.
				//During the next step, we can check if this flag was set, and if so, do reflection
				//checking.  If it didn't, but the flag is set from the previous step, unset it.
				if ( curStep.GetStepStatus() == "GeomBoundary" ) LastStepEndedOnBoundary = true;
				else if ( LastStepEndedOnBoundary ) LastStepEndedOnBoundary = false;
			}
			
			//We've done all of the necessary steps, so on to filling the tree and
			//finishing the iteration.
			theResultingTree->Fill();
			thePhoton.Reset();	
			//To finish this iteration, just increment the map iterator and
			//the track counter.
			track_it++;
			track_index++;
		}
	}
		
	//Now just return the tree we built.
#ifndef CLUSTER_RUN
#endif
	return theResultingTree;
}

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
		for ( int i = 0; i < TOTracksIterator->GetMCTrackStepCount(); i++ )
		{
			*theJoinedTrack.AddNewMCTrackStep() = *TOTracksIterator->GetMCTrackStep(i); 
		}
		TOTracksIterator++;
	}
	
	return theJoinedTrack;
}
