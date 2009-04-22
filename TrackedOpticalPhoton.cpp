#include "TrackedOpticalPhoton.hpp"

ClassImp(TrackedOpticalPhoton);

TrackedOpticalPhoton::TrackedOpticalPhoton() :
	fEventNo(0),
	fParentID(0),
	isCerenkov(false),
	isScintillation(false),
	isReemitted(false),
	ReemissionCount(0),
	hitCathode(false),
	triggeredDAQ(false)
{ }

void TrackedOpticalPhoton::Reset()
{
	fEventNo = 0;
	fParentID = 0;
	hitCathode = false;
	triggeredDAQ = false;
	isCerenkov = false;
	isReemitted = false;
	isScintillation = false;
	ReemissionCount = 0;
	while ( fReflectionX.empty() == false )
	{ fReflectionX.pop_back(); }
	while ( fReflectionY.empty() == false )
	{ fReflectionY.pop_back(); }
	while ( fReflectionZ.empty() == false )
	{ fReflectionZ.pop_back(); }
	while ( fReflectionT.empty() == false )
	{ fReflectionT.pop_back(); }
	while ( fPMTHitX.empty() == false )
	{ fPMTHitX.pop_back(); }
	while ( fPMTHitY.empty() == false )
	{ fPMTHitY.pop_back(); }
	while ( fPMTHitZ.empty() == false )
	{ fPMTHitZ.pop_back(); }
	while ( fPMTHitT.empty() == false )
	{ fPMTHitT.pop_back(); }
}

bool TrackedOpticalPhoton::CheckIntegrity() const
{
	bool integrity = true;

	if ( 3*fReflectionX.size() - (fReflectionT.size() + fReflectionY.size() + fReflectionZ.size()) != 0 ) integrity = false;
	if ( 3*fPMTHitX.size() - (fPMTHitY.size() + fPMTHitZ.size() + fPMTHitT.size()) != 0 ) integrity = false;
	return integrity;
}

bool TrackedOpticalPhoton::CheckIntegrity()
{
	return static_cast<const TrackedOpticalPhoton&>(*this).CheckIntegrity();
}

bool TrackedOpticalPhoton::QueuesEmpty() const
{
	if ( !fReflectionT.empty() || !fReflectionX.empty() || !fReflectionY.empty() || !fReflectionY.empty() ) return false;
	if ( !fPMTHitX.empty() || !fPMTHitY.empty() || !fPMTHitZ.empty() || !fPMTHitT.empty() ) return false;
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

bool TrackedOpticalPhoton::AddReflection( float rX, float rY, float rZ, float rT )
{
	fReflectionX.push_back(rX);
	fReflectionY.push_back(rY);
	fReflectionZ.push_back(rZ);
	fReflectionT.push_back(rT);

	return this->CheckIntegrity();
}

float TrackedOpticalPhoton::FirstReflectionTime() const
{
	assert( this->Reflected() );
	float _time = fReflectionT.front();
	return _time;	
}

float TrackedOpticalPhoton::FirstReflectionTime()
{
	return static_cast<const TrackedOpticalPhoton&>(*this).FirstReflectionTime();
}

float TrackedOpticalPhoton::LastReflectionTime() const
{
	assert( this->Reflected() );
	float _time = fReflectionT.back();
	return _time;
}

float TrackedOpticalPhoton::LastReflectionTime()
{
	return static_cast<const TrackedOpticalPhoton&>(*this).LastReflectionTime();
}

TVector3 TrackedOpticalPhoton::GetReflection(unsigned index) const
{
	assert( index < fReflectionT.size() );
	return *(new TVector3(fReflectionX[index],fReflectionY[index],fReflectionZ[index]) );
}

TVector3 TrackedOpticalPhoton::GetReflection(unsigned index)
{
	return static_cast<const TrackedOpticalPhoton&>(*this).GetReflection(index);
}

float TrackedOpticalPhoton::GetReflectionTime(unsigned index) const
{
	assert( index < fReflectionT.size() );
	float _time = fReflectionT[index];
	return _time;
}

float TrackedOpticalPhoton::GetReflectionTime(unsigned index)
{
	return static_cast<const TrackedOpticalPhoton&>(*this).GetReflectionTime(index);
}

float TrackedOpticalPhoton::GetFirstPMTHitTime() const
{
	assert( this->IndefiniteHit() );
	float _time = fPMTHitT[0];
	return _time;
}

float TrackedOpticalPhoton::GetFirstPMTHitTime()
{
	return static_cast<const TrackedOpticalPhoton&>(*this).GetFirstPMTHitTime();
}

float TrackedOpticalPhoton::GetPMTHitTime(unsigned index) const
{
	assert( index < fPMTHitT.size() );
	float _time = fPMTHitT[index];
	return _time;
}

float TrackedOpticalPhoton::GetPMTHitTime(unsigned index)
{
	return static_cast<const TrackedOpticalPhoton&>(*this).GetPMTHitTime(index);
}

bool TrackedOpticalPhoton::AddPMTHit(float hX, float hY, float hZ, float hT)
{
	if ( !hitCathode ) hitCathode = true;
	fPMTHitX.push_back(hX);
	fPMTHitY.push_back(hY);
	fPMTHitZ.push_back(hZ);
	fPMTHitT.push_back(hT);
	return this->CheckIntegrity();
}

//OStream utility functions.
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

bool TrackedOpticalPhoton::IndefiniteHit() const
{
	if ( hitCathode ) return true;
	else return false;
} 

bool TrackedOpticalPhoton::IndefiniteHit()
{
	return static_cast<const TrackedOpticalPhoton&>(*this).IndefiniteHit();
}

bool TrackedOpticalPhoton::DefiniteHit() const
{
	if ( triggeredDAQ ) return true;
	else return false;
}

bool TrackedOpticalPhoton::DefiniteHit()
{
	return static_cast<const TrackedOpticalPhoton&>(*this).DefiniteHit();
}
