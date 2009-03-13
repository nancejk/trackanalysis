/** @class DS::MCTrackStep
 *  Data Structure: Segment of particle track
 *
 *  @author Stan Seibert <volsung@physics.utexas.edu>
 *
 *  This class holds one step along the trajectory of a particle.
 *  The initial point in a track is represented with a MCTrackStep
 *  of length zero.  All other steps are defined "looking backwards",
 *  with the information in this class corresponding to the endpoint
 *  of the segment.
 *
 *  Particle transport is performed in GEANT4 so that a single step
 *  cannot cross a volume boundary, so each step is fully contained
 *  within a particular volume in the detector geometry.
 */
#ifndef __RAT_DS_MCTrackStep__
#define __RAT_DS_MCTrackStep__

#include <TObject.h>
#include <string>
#include <TVector3.h>

namespace RAT {
  namespace DS {

class MCTrackStep : public TObject {
public:
  MCTrackStep() { };
  virtual ~MCTrackStep() { };

  /** Length of this step.
   *
   *  If zero length, this step represents the beginning of a track. */
  virtual float GetLength() const { return length; };
  virtual void SetLength(float _length) { length = _length; };

  /** Position of endpoint of step (mm). */
  virtual const TVector3& GetEndpoint() const { return endpoint; };
  virtual void SetEndpoint(const TVector3 &_endpoint) { endpoint = _endpoint; };

  /** Time since start of event, in lab frame (ns). */
  virtual double GetGlobalTime() const { return globalTime; };
  virtual void SetGlobalTime(double _globalTime) { globalTime = _globalTime; };

  /** Time since start of track, in lab frame (ns). */
  virtual double GetLocalTime() const { return localTime; };
  virtual void SetLocalTime(double _localTime) { localTime = _localTime; };

  /** Time since start of track, in particle frame
      (ns). */
  virtual double GetProperTime() const { return properTime; };
  virtual void SetProperTime(double _properTime) { properTime = _properTime; };

  /** Momentum of particle (MeV/c) */
  virtual const TVector3& GetMomentum() const { return mom; };
  virtual void SetMomentum(const TVector3 &_mom) { mom = _mom; };
  
  /** Kinetic energy of particle (MeV) */
  virtual float GetKE() const { return ke; };
  virtual void SetKE(float _ke) { ke = _ke; };

  /** Name of physics process acting at endpoint. */
  virtual std::string GetProcess() const { return process; };
  virtual void SetProcess(const std::string &_process) { process = _process; };

  /** Name of detector volume in which this step took place. */
  virtual std::string GetVolume() const { return volume; };
  virtual void SetVolume(const std::string &_volume) { volume = _volume; };

  /** G4StepStatus at the step endpoint */
  virtual std::string GetStepStatus() const { return stepStatus; };
  virtual void SetStepStatus(const std::string &_status) { stepStatus = _status; };

  ClassDef(MCTrackStep,2)
  
protected:
  float length;
  TVector3 endpoint;
  double globalTime;
  double localTime;
  double properTime;
  TVector3 mom;
  double ke;  
  std::string process;
  std::string volume;
  std::string stepStatus;
};

  } // namespace DS
} // namespace RAT

#endif
