#ifndef PROFILER_H
#define PROFILER_H

#include <Core/DataTypes.h>
#include <Core/Array.h>

#include <string>
#include <map>

///////////////////////////////////////////////////////////////////////////////

class ProfilerMarker
{
public:
	ProfilerMarker(const char* name, const char* fname, Uint32 lineNum, Uint32 id);
	~ProfilerMarker();

	/* Stop recording time */
	void Stop();

	/* Get name */
	const char* GetName() const;
	/* Get file name marker is in */
	const char* GetFileName() const;
	/* Get line number of marker */
	Uint32 GetLineNum() const;
	/* Get marker ID */
	Uint32 GetID() const;
	/* Get total runtime */
	Uint64 GetTotalTime() const;

private:
	/* Name of the marker */
	const char* mName;
	/* File that marker was put in */
	const char* mFileName;
	/* Line of code of the marker */
	Uint32 mLineNum;

	/* Marker ID */
	Uint32 mID;

	/* Start time */
	Uint64 mStartTime;
	/* Total time */
	Uint64 mTotalTime;
	/* True if profiler is active for this marker */
	bool mIsActive;
};

///////////////////////////////////////////////////////////////////////////////

/* Keeps track of data for each profiler marker */
struct ProfilerData
{
	/* List of total times */
	Array<Uint64> mTotalTimes;
	/* List of number of times each marker has run */
	Array<Uint32> mNumRuns;

	/* Name of the marker */
	const char* mName;
	/* File that marker was put in */
	const char* mFileName;
	/* Line of code of the marker */
	Uint32 mLineNum;
};

///////////////////////////////////////////////////////////////////////////////

class Profiler
{
public:
	/* Record data from marker */
	static void RecordMarker(const ProfilerMarker& marker);

	/* Log data to file */
	static void Log(const char* fname);

private:
	/* Profiler data */
	static std::map<std::string, ProfilerData> mData;
};

///////////////////////////////////////////////////////////////////////////////

#include <Core/Macros.h>

#define _PROFILE_NO_ID(name) ProfilerMarker CONCAT(Profiler_, name)(STR(name), FILE_NAME, __LINE__, 0);
#define _PROFILE_ID(name, id) ProfilerMarker CONCAT(Profiler_, name)(STR(name), FILE_NAME, __LINE__, id);

#define _CHOOSE_PROFILER_FUNC(a, b, x, ...) x
#define START_PROFILER(...) EXPAND(_CHOOSE_PROFILER_FUNC(__VA_ARGS__, _PROFILE_ID, _PROFILE_NO_ID)(__VA_ARGS__))

#define STOP_PROFILER(name) \
CONCAT(Profiler_, name).Stop(); \
Profiler::RecordMarker(CONCAT(Profiler_, name));

///////////////////////////////////////////////////////////////////////////////

#endif