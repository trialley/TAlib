#ifndef _ASYNC_LOGGING_HH
#define _ASYNC_LOGGING_HH

#include <memory>
#include <mutex>
#include <string>

#include <Logger/Thread.hpp>
#include "LogStream.hpp"
#include <Logger/ptr_vector.hpp>
#include <Logger/Condition.hpp>

class AsyncLogging
{
public:
	AsyncLogging(const std::string filePath, off_t rollSize, double flushInterval = 3.0);
	~AsyncLogging();

	void start(){
		m_isRunning = true;
		m_thread.start();
	}

	void stop(){
		m_isRunning = false;
		m_cond.notify();
		m_thread.join();
	}

	void append(const char *logline, std::size_t len);

private:
	AsyncLogging(const AsyncLogging&);
	AsyncLogging& operator=(const AsyncLogging&);

	void threadRoutine();

	typedef LogBuffer<kLargeBuffer> Buffer;
	typedef myself::ptr_vector<Buffer> BufferVector;
	typedef std::unique_ptr<Buffer> BufferPtr;

	std::string m_filePath;
	off_t m_rollSize;
	const double m_flushInterval;
	bool m_isRunning;
	Thread m_thread;
	std::mutex m_mutex;
	Condition m_cond;

	BufferPtr m_currentBuffer;
	BufferPtr m_nextBuffer;
	BufferVector m_buffers;
};

#endif
