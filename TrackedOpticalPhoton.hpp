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
	TrackedOpticalPhoton();
	void Reset();
	bool CheckIntegrity() const;
	bool CheckIntegrity();
	bool QueuesEmpty() const;
	bool QueuesEmpty();
	bool Reflected() const;
	bool Reflected();
	unsigned ReflectionCount() const;
	unsigned ReflectionCount();
	TVector3 FirstReflection() const;
	TVector3 FirstReflection();
	TVector3 LastReflection() const;
	TVector3 LastReflection();
	bool AddReflection(float,float,float);
	friend std::ostream& operator << (std::ostream&, const TrackedOpticalPhoton&);
	friend std::ostream& operator << (std::ostream&, TrackedOpticalPhoton&);
private:
	unsigned fEventNo;
	std::deque<float> fReflectionX;
	std::deque<float> fReflectionY;
	std::deque<float> fReflectionZ;
//------------//
//ROOT business
ClassDef(TrackedOpticalPhoton,1);
};
