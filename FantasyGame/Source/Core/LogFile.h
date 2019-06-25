#ifndef LOG_FILE_H
#define LOG_FILE_H

#include <iostream>
#include <fstream>

///////////////////////////////////////////////////////////////////////////////

class LogFile
{
public:
	static LogFile gLogFile;

public:
	LogFile(const char* fname);
	~LogFile();

	template <typename T>
	LogFile& operator<<(T val)
	{
		mFile << val;
#if _DEBUG
		std::cout << val;
#endif
		return *this;
	}

private:
	/* Log file */
	std::ofstream mFile;
};

///////////////////////////////////////////////////////////////////////////////

#define LOG LogFile::gLogFile
#define LOG_WARNING LOG << "WARNING : "
#define LOG_ERROR LOG << "ERROR : "

///////////////////////////////////////////////////////////////////////////////

#endif