#pragma once
#include <chrono>

#define steady_clock std::chrono::steady_clock

///Wrapper class around std::chrono for time related features
class ChiliTimer
{
public:
	ChiliTimer() noexcept;
	float Mark() noexcept;
	float Peek() const noexcept;
private:
	steady_clock::time_point last;
};

