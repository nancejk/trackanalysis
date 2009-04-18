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
	bool CheckIntegrity();
	bool QueuesEmpty();
private:
	unsigned fEventNo;
	std::deque<float> fReflectionX;
	std::deque<float> fReflectionY;
	std::deque<float> fReflectionZ;
//------------//
//ROOT business
ClassDef(TrackedOpticalPhoton,1);
};
