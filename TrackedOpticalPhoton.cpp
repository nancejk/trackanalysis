#include "TrackedOpticalPhoton.hpp"

ClassImp(TrackedOpticalPhoton);

TrackedOpticalPhoton::TrackedOpticalPhoton() :
	fEventNo(0)
{ }

void TrackedOpticalPhoton::Reset()
{
	if ( this->CheckIntegrity() )
	{
		while ( this->QueuesEmpty() == false )
		{
			fReflectionX.pop_back();
			fReflectionY.pop_back();
			fReflectionZ.pop_back();
		}
	}
}

bool TrackedOpticalPhoton::CheckIntegrity()
{
	bool integrity = true;

	if ( 2*fReflectionX.size() - (fReflectionY.size() + fReflectionZ.size()) != 0 ) integrity = false;
	
	return integrity;
}

bool TrackedOpticalPhoton::QueuesEmpty()
{
	if ( !fReflectionX.empty() || !fReflectionY.empty() || !fReflectionY.empty() ) return false;
	return true;
}

