#pragma once
#include <chrono>
using namespace std;
using namespace chrono;
class Timer
{
public:
	Timer();
	double GetMiliSecondsElapsed();
	void Restart();
	bool Stop();
	bool Start();
	~Timer();

private:
	bool isRunning = false;
#ifdef _WIN32
	time_point<steady_clock> start;
	time_point<steady_clock> stop;
#else
	chrono<system_clock> start;
	chrono<system_clock> stop;
#endif
};

