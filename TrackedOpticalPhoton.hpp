#ifndef __TROPTPH_DEF
#define __TROPTPH_DEF
//C++ Includes
#include <queue> //For deque
#include <map>   //For track merging

//ROOT Includes
#include "TObject.h" //Inheritance
#include "TTree.h" //For I/O
#include "TVector3.h" //For interfacing with ROOT

class TrackedOpticalPhoton : public TObject
{
public:
	/*The default constructor.  There really isn't a need
	for a non-default constructor at this point*/
	TrackedOpticalPhoton();
	/*Resets all internal values to zero and clears internal
	caches.  There is no const-correct version for obvious
	reasons.*/
	void Reset();
	/*Returns true if all queues are of equal length.*/
	bool CheckIntegrity() const;
	bool CheckIntegrity();
	/*Returns true is all queues are empty of data.*/
	bool QueuesEmpty() const;
	bool QueuesEmpty();
	/*Returns true if this photon was marked as reflected.*/
	bool Reflected() const;
	bool Reflected();
	/*Returns true if this photon hit a photocathode during
	the Monte Carlo.*/
	bool IndefiniteHit() const;
	bool IndefiniteHit();
	/*Returns true if this photon hit a photocathode during
	the Monte Carlo simulation AND caused the frontend to
	be triggered.  DefiniteHits will show up in RAT as 
	MCPMTHits.*/
	void MarkDefiniteHit() { triggeredDAQ = true; };
	bool DefiniteHit() const;
	bool DefiniteHit();
	/*The number of times this photon was counted as having
	reflected from any surface.*/
	unsigned ReflectionCount() const;
	unsigned ReflectionCount();
	/*Constructs and returns a TVector3 whose XYZ components 
	mark the place where the first reflection this track had
	occurred.*/
	TVector3 FirstReflection() const;
	TVector3 FirstReflection();
	/*Constructs and returns a TVector3 whose XYZ components 
	mark the last reflection point on the track.*/
	TVector3 LastReflection() const;
	TVector3 LastReflection();
	/*Returns the global time (since the start of the simulation)
	that the first reflection occurred for this track (if it did).
	If the track never reflected, an assert() will fail in this 
	loop, which will kill whatever process (including ROOT) is
	running.  Therefore, this should emphatically NOT be used
	as a check if a photon was a reflection.  This is also to
	avoid confusion, as it is impossible for a photon to have
	a reflection time of zero this way.*/
	float FirstReflectionTime() const;
	float FirstReflectionTime();
	/*Returns the global time (since the start of the simulation)
	that the first reflection occurred for this track (if it did).
	If the loop never reflected, an assert() will fail in this 
	method, which will kill whatever process (including ROOT) is
	calling it.  Therefore, this should emphatically NOT be used
	as a check if a photon was a reflection.  This is also to avoid
	confusion, as it is impossible for a photon to have a reflection
	time of zero this way.  For safety, always use the Reflected() 
	method to determine if a track was reflected.*/	
	float LastReflectionTime() const;
	float LastReflectionTime();
	/*Constructs and returns a TVector3 which marks the position of
	the i'th reflection, where i is the argument to this method.
	assert() will fail if i is greater than this->ReflectionCount().*/
	TVector3 GetReflection(unsigned) const;
	TVector3 GetReflection(unsigned);
	/*Returns the time of the i'th reflection, where i is the argument
	to this method.  assert() will fail if i is greater than
	this->ReflectionCount().*/
	float GetReflectionTime(unsigned) const;
	float GetReflectionTime(unsigned);
	/*Adds a reflection to *this.  No const method is provided for
	obvious reasons.*/
	bool AddReflection(float,float,float,float);
	/*Returns the time of the first photocathode hit for this photon.
	assert() will fail if this photon never hit a PMT.*/
	float GetFirstPMTHitTime() const;
	float GetFirstPMTHitTime();
	/*Returns the time of the i'th PMT Hit (there is usually only one),
	where i is the argument to this method.  assert() will fail if
	i is greater than this->HitCount().*/
	float GetPMTHitTime(unsigned) const;
	float GetPMTHitTime(unsigned);
	/*Adds a PMT Hit in (XYZT) coordinates.  If the TrackedOpticalPhoton
	retains its integrity after the add, this returns true.*/
	bool AddPMTHit(float,float,float,float);
	/*Inocuous information about when and by whom this photon was
	generated.*/
	unsigned fEventNo;
	/*The Monte Carlo event number of the run in which this photon was generated.*/
	unsigned fParentID;	
	/*std::ostream helper functions to display track data through
	a stream.*/
	friend std::ostream& operator << (std::ostream&, const TrackedOpticalPhoton&);
	friend std::ostream& operator << (std::ostream&, TrackedOpticalPhoton&);
	/*Booleans for processes.*/
	bool isCerenkov;
	bool isScintillation;
	bool isReemitted;
	/*Reemission counter.*/
	unsigned ReemissionCount;
private:
	/*The private data stores of this object.  Deques are used instead of vectors
	for their increased efficiency in adding and deleting members at the ends.*/
	std::deque<float> fReflectionX;
	std::deque<float> fReflectionY;
	std::deque<float> fReflectionZ;
	std::deque<float> fReflectionT;
	/*Data stores for PMT hits.*/
	std::deque<float> fPMTHitX;
	std::deque<float> fPMTHitY;
	std::deque<float> fPMTHitZ;
	std::deque<float> fPMTHitT;
	/*Booleans for PMT hit information.*/
	bool hitCathode;
	bool triggeredDAQ;
	
//------------//
//ROOT business
ClassDef(TrackedOpticalPhoton,1);
};
#endif
