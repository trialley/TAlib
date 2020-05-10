#include "AsyncLogging.h"

#include <assert.h>
#include <stdio.h>

#include "LogFile.h"

AsyncLogging* AsyncLogging::getInstance() {
	static AsyncLogging sington;
	return &sington;
}
AsyncLogging::AsyncLogging()
	: m_filePath(),
	  m_rollSize(10),
	  m_flushInterval(3),
	  m_isRunning(false),
	  m_thread(std::bind(&AsyncLogging::threadRoutine, this)),
	  m_mutex(),
	  m_cond(),
	  m_currentBuffer(new Buffer),
	  m_nextBuffer(new Buffer),
	  m_buffers() {
}

AsyncLogging::~AsyncLogging() {
	if (m_isRunning)
		stop();
}

void AsyncLogging::append(const char* logline, std::size_t len) {
	std::lock_guard<std::mutex> lock(m_mutex);
	if (m_currentBuffer->avail() > len) {				 //如果剩余空间足够
		m_currentBuffer->append(logline, len);			 //添加数据
	} else {											 //没有剩余
		m_buffers.push_back(m_currentBuffer.release());	 //当前buffer被推入队列，m_buffers代表当前正在写入的队列

		if (m_nextBuffer) {	 //若有空闲buffer则用起来
			m_currentBuffer = std::move(m_nextBuffer);
		} else {  //没有则新申请一个
			m_currentBuffer.reset(new Buffer);
		}

		m_currentBuffer->append(logline, len);
		m_cond.notify();
	}
}

void AsyncLogging::threadRoutine() {
	assert(m_isRunning == true);
	LogFile output(m_filePath, m_rollSize, false);
	BufferPtr backupBuffer1(new Buffer);
	BufferPtr backupBuffer2(new Buffer);
	BufferVector buffersToWrite;
	buffersToWrite.reserve(8);

	while (m_isRunning) {
		assert(buffersToWrite.empty());
		{
			std::unique_lock<std::mutex> lock(m_mutex);
			if (m_buffers.empty()) {  //用户队列空，等一会
				m_cond.waitForSeconds(lock, m_flushInterval);
			}
			m_buffers.push_back(m_currentBuffer.release());	 //释放当前收集缓冲区，
			m_currentBuffer = std::move(backupBuffer1);		 //当前buffer使用后备buffer
			m_buffers.swap(buffersToWrite);					 //交换用户和刷新两个缓冲队列
			if (!m_nextBuffer)								 //添加下一个空闲缓冲区
				m_nextBuffer = std::move(backupBuffer2);
		}

		assert(!buffersToWrite.empty());  //待写入为空可不行

		for (size_t i = 0; i < buffersToWrite.size(); ++i) {
			output.append(buffersToWrite[i]->data(), buffersToWrite[i]->length());
		}

		if (buffersToWrite.size() > 2) {
			// drop non-bzero-ed buffers, avoid trashing
			buffersToWrite.resize(2);
		}

		if (!backupBuffer1) {
			assert(!buffersToWrite.empty());
			backupBuffer1 = std::move(buffersToWrite.pop_back());
			backupBuffer1->reset();
		}

		if (!backupBuffer2) {
			assert(!buffersToWrite.empty());
			backupBuffer2 = std::move(buffersToWrite.pop_back());
			backupBuffer2->reset();
		}

		buffersToWrite.clear();	 //全部写完，现在清空所有剩余缓冲区
		output.flush();
	}

	output.flush();
}
