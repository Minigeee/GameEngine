#ifndef THREAD_H
#define THREAD_H

#include <thread>
#include <mutex>
#include <future>

///////////////////////////////////////////////////////////////////////////////

/* std::thread wrapper */
class Thread
{
public:
	Thread();
	~Thread();

	/* Run function */
	template <typename Func, typename... Args>
	void Run(Func&& func, Args&&... args)
	{
		mThread = std::thread(func, args...);
		mIsActive = true;
	}

	/* Join thread */
	void Join();

private:
	std::thread mThread;
	/* True if thread is active */
	bool mIsActive;
};

///////////////////////////////////////////////////////////////////////////////

/* std::mutex wrapper */
class Mutex
{
public:
	Mutex();
	~Mutex();

	/* Lock mutex */
	void Lock();
	/* Unlock mutex */
	void Unlock();

private:
	std::mutex mMutex;
};

///////////////////////////////////////////////////////////////////////////////

/* Handles locking/unlocking for mutex */
class Lock
{
public:
	Lock(Mutex& mutex);
	~Lock();

private:
	Mutex& mMutex;
};

///////////////////////////////////////////////////////////////////////////////

/* Simple synchronizationa and communication between threads */
template <typename T>
class SyncPoint
{
public:
	SyncPoint() { Reset(); }
	~SyncPoint() { }

	/* Reset sync point */
	void Reset()
	{
		mPromise = std::promise<T>();
		mFuture = mPromise.get_future();
	}

	/* Set value */
	void Set(const T& val)
	{
		mPromise.set_value(val);
	}

	/* Get value */
	T Get()
	{
		T val = mFuture.get();
		Reset();
		return val;
	}

private:
	std::promise<T> mPromise;
	std::future<T> mFuture;
};

///////////////////////////////////////////////////////////////////////////////

#endif