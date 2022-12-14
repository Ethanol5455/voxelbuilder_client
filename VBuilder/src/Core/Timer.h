#pragma once

#include "Time.h"

#include <chrono>


class Timer
{
public:
	Timer(bool printOnDelete = false);
	~Timer();

	Time GetElapsedTime();

	void Restart();

private:
	std::chrono::time_point<std::chrono::high_resolution_clock> m_StartTimepoint;

	bool m_PrintOnDelete;
};