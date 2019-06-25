#ifndef CLOCK_H
#define CLOCK_H

#include <Core/DataTypes.h>

///////////////////////////////////////////////////////////////////////////////

/* Measures time elapsed */
class Clock
{
public:
	Clock();

	/* Get time (in seconds) since construction or last restart */
	float GetElapsedTime() const;

	/* Reset clock and get elapsed time (in seconds) */
	float Restart();

private:
	/* Time of last restart (microseconds) */
	Uint64 mStartTime;
};

///////////////////////////////////////////////////////////////////////////////

#endif