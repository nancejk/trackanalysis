/*
 *  TrackAnalysisTreeOutputObject.h
 *  RAT_track_extend
 *
 *  Created by Jared Nance on 1/5/09.
 *  Copyright 2009 University of Washington. All rights reserved.
 *
 */

#include <queue>
#include <numeric>
#include <TTree.h>
#include "RAT/DSReader.hh"
#include "RAT/DS/Root.hh"
#include "RAT/TrackNav.hh"
#include "RAT/TrackCursor.hh"

//This is the function that will take a RAT data structure and produce a tree containing all of the data
//from the optical photon tracks inside the DS.
TTree* GrowPhotonTree( RAT::DSReader& );

//An enumerated data type that will allow us to remember processes as hexadecimal numbers.
//The idea is that you just add to the 'history' of the photon whatever number corresponds
//to that type of event - history += eReflection adds a reflection to the memory of the photon.
enum processes 
{
	eReflection = 7,
	eReemission = 11,
	eScattering = 17,
	ePMTHit = 23
};

struct TrackedOpticalPhoton
{
	unsigned eventNo;
	unsigned parentID;
	float fGenerationTime;
	float fGenerationRadius;
	float fGenerationEnergy;
	float fPMTHitTime;
	float fPMTHitEnergy;
	bool defHit;
	bool indefHit;
	bool reemitted;
	bool cerenkov;
	bool scintillation;
};

void ClearTrackedPhoton(TrackedOpticalPhoton&);
