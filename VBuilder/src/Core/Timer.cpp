#include "vbpch.h"
#include "Timer.h"

Timer::Timer(bool printOnDelete)
	: m_PrintOnDelete(printOnDelete)
{
	Restart();
}

Timer::~Timer()
{
	if (m_PrintOnDelete) {
		float time = GetElapsedTime().AsMilliseconds();
		VB_INFO("Timer ended after {0} ms", time);
	}
}

Time Timer::GetElapsedTime()
{
	auto endTimepoint = std::chrono::high_resolution_clock::now();

	auto start = std::chrono::time_point_cast<std::chrono::microseconds>(
			     m_StartTimepoint)
			     .time_since_epoch()
			     .count();
	auto end = std::chrono::time_point_cast<std::chrono::microseconds>(
			   endTimepoint)
			   .time_since_epoch()
			   .count();

	auto duration = end - start;

	return { duration };
}

void Timer::Restart()
{
	m_StartTimepoint = std::chrono::high_resolution_clock::now();
}