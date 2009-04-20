#include "TrackedOpticalPhoton.hpp"

ClassImp(TrackedOpticalPhoton);

TrackedOpticalPhoton::TrackedOpticalPhoton() :
	fEventNo(0)
{ }

void TrackedOpticalPhoton::Reset()
{
	while ( this->QueuesEmpty() == false )
	{
		fReflectionX.pop_back();
		fReflectionY.pop_back();
		fReflectionZ.pop_back();
	}
}

bool TrackedOpticalPhoton::CheckIntegrity() const
{
	bool integrity = true;

	if ( 2*fReflectionX.size() - (fReflectionY.size() + fReflectionZ.size()) != 0 ) integrity = false;
	
	return integrity;
}

bool TrackedOpticalPhoton::CheckIntegrity()
{
	return static_cast<const TrackedOpticalPhoton&>(*this).CheckIntegrity();
}

bool TrackedOpticalPhoton::QueuesEmpty() const
{
	if ( !fReflectionX.empty() || !fReflectionY.empty() || !fReflectionY.empty() ) return false;
	return true;
}

bool TrackedOpticalPhoton::QueuesEmpty()
{
	return static_cast<const TrackedOpticalPhoton&>(*this).QueuesEmpty();
}

TVector3 TrackedOpticalPhoton::FirstReflection() const
{
	assert( this->Reflected() );
	return *(new TVector3(fReflectionX[0],fReflectionY[0],fReflectionZ[0] ) );
}

TVector3 TrackedOpticalPhoton::FirstReflection()
{
	return static_cast<const TrackedOpticalPhoton&>(*this).FirstReflection();
}

TVector3 TrackedOpticalPhoton::LastReflection() const
{
	assert( this->Reflected() );
	return *(new TVector3(fReflectionX.back(), fReflectionY.back(), fReflectionZ.back()));
}

TVector3 TrackedOpticalPhoton::LastReflection()
{
	return static_cast<const TrackedOpticalPhoton&>(*this).LastReflection();
}

unsigned TrackedOpticalPhoton::ReflectionCount() const
{
	//Make sure this object makes sense.
	assert( this->CheckIntegrity() );
	
	return fReflectionX.size();
}

unsigned TrackedOpticalPhoton::ReflectionCount()
{
	return static_cast<const TrackedOpticalPhoton&>(*this).ReflectionCount();
}

bool TrackedOpticalPhoton::Reflected() const
{
	if ( fReflectionX.size() != 0 ) return true;
	return false;
}

bool TrackedOpticalPhoton::Reflected()
{
	return static_cast<const TrackedOpticalPhoton&>(*this).Reflected();
}

bool TrackedOpticalPhoton::AddReflection( float rX, float rY, float rZ )
{
	fReflectionX.push_back(rX);
	fReflectionY.push_back(rY);
	fReflectionZ.push_back(rZ);

	return this->CheckIntegrity();
}

std::ostream& operator << (std::ostream& lhs, const TrackedOpticalPhoton& rhs)
{
	lhs << std::boolalpha << "Integrity is " << rhs.CheckIntegrity() << std::endl;
	lhs << rhs.ReflectionCount() << " reflections counted." << std::endl;
	return lhs;
}

std::ostream& operator << (std::ostream& lhs, TrackedOpticalPhoton& rhs)
{
	return ( lhs << static_cast<const TrackedOpticalPhoton&>(rhs) ); 
} 
