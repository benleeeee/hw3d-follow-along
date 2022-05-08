#include "ChiliTimer.h"

ChiliTimer::ChiliTimer() noexcept
{
	last = steady_clock::now();
}

///Returns time elapsed since the last time Mark() was called
float ChiliTimer::Mark() noexcept
{
	const auto old = last;	//cache old time
	last = steady_clock::now(); //set last to current time
	const std::chrono::duration<float> frameTime = last - old; //calc difference
	return frameTime.count();
}

///Returns time elapsed since last time Mark() was called without resetting time
float ChiliTimer::Peek() const noexcept
{
	return std::chrono::duration<float>(steady_clock::now() - last).count();
}