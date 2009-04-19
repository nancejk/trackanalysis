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

TVector3 TrackedOpticalPhoton::FirstReflection()
{
	assert( this->Reflected() );
	return *(new TVector3(fReflectionX[0],fReflectionY[0],fReflectionZ[0] ) );
}

TVector3 TrackedOpticalPhoton::LastReflection()
{
	assert( this->Reflected() );
	return *(new TVector3(fReflectionX.back(), fReflectionY.back(), fReflectionZ.back()));
}

unsigned TrackedOpticalPhoton::ReflectionCount()
{
	//Make sure this object makes sense.
	assert( this->CheckIntegrity() );
	
	return fReflectionX.size();
}

bool TrackedOpticalPhoton::Reflected()
{
	if ( fReflectionX.size() != 0 ) return true;
	return false;
}

bool TrackedOpticalPhoton::AddReflection( float rX, float rY, float rZ )
{
	fReflectionX.push_back(rX);
	fReflectionY.push_back(rY);
	fReflectionZ.push_back(rZ);

	return this->CheckIntegrity();
}
