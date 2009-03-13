#include <RAT/Trajectory.hh>
#include <RAT/Extensible.hh>
#include <G4StepPoint.hh>
#include <G4VProcess.hh>
#include <G4VPhysicalVolume.hh>
#include <RAT/Log.hh>

namespace RAT {


G4Allocator<Trajectory> aTrajectoryAllocator;

Trajectory::Trajectory() : G4Trajectory()
{
  ratTrack = Extensible<DS::MCTrack>::New();
}

Trajectory::Trajectory(const G4Track *aTrack) : G4Trajectory(aTrack),
						      creatorProcessName("start")
{
  ratTrack = Extensible<DS::MCTrack>::New();
  
  ratTrack->SetTrackID(GetTrackID());
  ratTrack->SetParentID(GetParentID());
  ratTrack->SetPDGCode(GetPDGEncoding());
  ratTrack->SetParticleName(GetParticleName());
  
  const G4VProcess *creatorProcess = aTrack->GetCreatorProcess();
  if (creatorProcess)
    creatorProcessName = creatorProcess->GetProcessName();

  ratTrack->SetLength(0.0);
}

Trajectory::~Trajectory()
{
  delete ratTrack;
}

void Trajectory::AppendStep(const G4Step* aStep)
{
  if (ratTrack->GetMCTrackStepCount() == 0) {
    // Add initial step at very beginning of track
    DS::MCTrackStep *initStep = ratTrack->AddNewMCTrackStep();
    G4StepPoint *initPoint = aStep->GetPreStepPoint();
    FillStep(initPoint, aStep, initStep, 0.0);
  }

  DS::MCTrackStep *ratStep = ratTrack->AddNewMCTrackStep();
  G4StepPoint *endPoint = aStep->GetPostStepPoint();
  FillStep(endPoint, aStep, ratStep, aStep->GetStepLength());  
  // Update total track length
  ratTrack->SetLength(ratTrack->GetLength() + ratStep->GetLength());
}

void Trajectory::FillStep(const G4StepPoint *point, const G4Step *step,
                          DS::MCTrackStep *ratStep, double stepLength)
{
  G4StepPoint *startPoint = step->GetPreStepPoint();

  ratStep->SetLength(stepLength);

  const G4ThreeVector &pos = point->GetPosition();
  ratStep->SetEndpoint( TVector3(pos.x(), pos.y(), pos.z()) );
  ratStep->SetGlobalTime(point->GetGlobalTime());
  ratStep->SetLocalTime(point->GetLocalTime());
  ratStep->SetProperTime(point->GetProperTime());

  G4ThreeVector mom = point->GetMomentum();
  ratStep->SetMomentum( TVector3(mom.x(), mom.y(), mom.z()) );
  ratStep->SetKE(point->GetKineticEnergy());

  switch ( point->GetStepStatus() )
  {
	case 0:
		//The step reached the World boundary.
		ratStep->SetStepStatus("WorldBoundary");
		break;
	case 1:
		//The step reached a geometric boundary.
		ratStep->SetStepStatus("GeomBoundary");
		break;
	case 2:
		//Step defined by a PreStepDoItVector.
		ratStep->SetStepStatus("AtRestDoItProc");
		break;
	case 3:
		//Step defined by an AlongStepDoItVector.
		ratStep->SetStepStatus("AlongStepDoItProc");
		break;
	case 4:
		//Step defined by a PostStepDoItVector.
		ratStep->SetStepStatus("PostStepDoItProc");
		break;
	case 5:
		//Step defined by the user Step Limit in the
		//logical volume.
		ratStep->SetStepStatus("UserDefinedLimit");
		break;
	case 6:
		//Step defined by an exclusively forced
		//PostStepDoIt process
		ratStep->SetStepStatus("ExclusivelyForcedProc");
		break;
	case 7:
		//Step not defined yet.
		ratStep->SetStepStatus("Undefined");
		break;
	default:
		//This should never happen in practice, but if
		//GetStepStatus() doesn't return something we
		//anticipated, set it to this neutral value.
		ratStep->SetStepStatus("Unknown");
		break;
  }  

  const G4VProcess *process = point->GetProcessDefinedStep();
  if (process == 0)
    ratStep->SetProcess(creatorProcessName); // Assume first step
  else
    ratStep->SetProcess(process->GetProcessName());

  G4VPhysicalVolume *volume = startPoint->GetPhysicalVolume();
  if(volume == NULL){
    detail<<"\nTrajectory encountered a NULL volume.  Continuing...\n";
    ratStep->SetVolume("NULL");
  }
  else{
    ratStep->SetVolume(volume->GetName());
  }
}


void Trajectory::MergeTrajectory(G4VTrajectory* secondTrajectory)
{
  G4Trajectory::MergeTrajectory(secondTrajectory);

  Trajectory *secondTraj = dynamic_cast<Trajectory*>(secondTrajectory);
  if (secondTraj) {
    for (int i=1; i < secondTraj->ratTrack->GetMCTrackStepCount(); i++)
      *ratTrack->AddNewMCTrackStep() = *secondTraj->ratTrack->GetMCTrackStep(i);
    secondTraj->ratTrack->PruneMCTrackStep();
  }
}


} // namespace RAT
