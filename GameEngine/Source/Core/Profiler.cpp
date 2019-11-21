#include <Core/Profiler.h>
#include <Core/Clock.h>
#include <Core/LogFile.h>

#include <fstream>

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

ProfilerMarker::ProfilerMarker(const char* name, const char* fname, Uint32 lineNum, Uint32 id) :
	mName			(name),
	mFileName		(fname),
	mLineNum		(lineNum),
	mID				(id),
	mStartTime		(ClockImpl()),
	mTotalTime		(0),
	mIsActive		(true)
{

}

ProfilerMarker::~ProfilerMarker()
{
	// Stop profiler when it is destroyed
	Stop();
	// Record data before destruction
	Profiler::RecordMarker(*this);
}

///////////////////////////////////////////////////////////////////////////////

void ProfilerMarker::Stop()
{
	if (mIsActive)
	{
		mTotalTime += ClockImpl() - mStartTime;
		mIsActive = false;
	}
}

///////////////////////////////////////////////////////////////////////////////

const char* ProfilerMarker::GetName() const
{
	return mName;
}

const char* ProfilerMarker::GetFileName() const
{
	return mFileName;
}

Uint32 ProfilerMarker::GetLineNum() const
{
	return mLineNum;
}

Uint32 ProfilerMarker::GetID() const
{
	return mID;
}

Uint64 ProfilerMarker::GetTotalTime() const
{
	return mTotalTime;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

std::map<std::string, ProfilerData> Profiler::mData;

///////////////////////////////////////////////////////////////////////////////

void Profiler::RecordMarker(const ProfilerMarker& marker)
{
	// Get marker data
	ProfilerData& data = mData[marker.GetName()];

	// Create data object if needed
	if (!data.mTotalTimes.Capacity())
	{
		data.mTotalTimes.Reserve(1);
		data.mNumRuns.Reserve(1);

		data.mName = marker.GetName();
		data.mFileName = marker.GetFileName();
		data.mLineNum = marker.GetLineNum();
	}

	// Create new data objects to fit ID
	Uint32 id = marker.GetID();
	if (id >= data.mTotalTimes.Size())
	{
		for (Uint32 i = data.mTotalTimes.Size(); i <= id; ++i)
		{
			data.mTotalTimes.Push(0);
			data.mNumRuns.Push(0);
		}
	}

	// Update values
	data.mTotalTimes[id] += marker.GetTotalTime();
	++data.mNumRuns[id];
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void Profiler::Log(const char* fname)
{
	// Open file
	std::ofstream f(fname);
	if (!f.is_open())
	{
		LOG_ERROR << "Could not open " << fname << "\n";
		return;
	}

	for (auto it = mData.begin(); it != mData.end(); ++it)
	{
		const ProfilerData& data = it->second;

		// Loop IDs
		for (Uint32 i = 0; i < data.mTotalTimes.Size(); ++i)
		{
			// Seperator
			f << "==================================\n";
			// Title
			f << data.mName << " - " << i << "\n";
			// Location info
			f << data.mFileName << " - Line " << data.mLineNum << "\n";

			// Data
			double mean = (double)(data.mTotalTimes[i] / data.mNumRuns[i]) / 1000.0;
			f << "Mean - " << mean << " ms\n";

			f << "\n";
		}
	}
}

///////////////////////////////////////////////////////////////////////////////