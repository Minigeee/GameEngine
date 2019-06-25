#include <Core/Sleep.h>
#include <Core/DataTypes.h>

///////////////////////////////////////////////////////////////////////////////

#ifdef WIN32

#include <Windows.h>

void sleepImpl(float seconds)
{
	TIMECAPS tc;
	timeGetDevCaps(&tc, sizeof(TIMECAPS));

	// Set timer resolution to lowest
	timeBeginPeriod(tc.wPeriodMin);

	::Sleep((DWORD)(seconds * 1000.0f));

	// Reset timer resolution
	timeEndPeriod(tc.wPeriodMin);
}

#endif

///////////////////////////////////////////////////////////////////////////////

void Sleep(float seconds)
{
	if (seconds > 0.0f)
		sleepImpl(seconds);
}