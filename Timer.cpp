#include "Timer.h"



Timer::Timer()
{
	start = high_resolution_clock::now();
	stop = high_resolution_clock::now();
}

double Timer::GetMiliSecondsElapsed()
{
	if (isRunning) 
	{
		auto elapsed = duration<double, milli>(high_resolution_clock::now() - start);
		return elapsed.count();
	}
	else
	{
		auto elapsed = duration<double, milli>(stop - start);
		return elapsed.count();
	}
}

void Timer::Restart()
{
	isRunning = true;
	start = high_resolution_clock::now();
}

bool Timer::Stop()
{
	if (!isRunning)
	{
		return false;
	}
	else
	{
		stop = high_resolution_clock::now();
		isRunning = false;
		return true;
	}
}

bool Timer::Start()
{
	if (isRunning)
	{
		return false;
	}
	else
	{
		start = high_resolution_clock::now();
		isRunning = true;
		return true;
	}
}


Timer::~Timer()
{
}
