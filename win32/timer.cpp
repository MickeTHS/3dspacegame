#include "timer.h"

Gfx_timer::Gfx_timer()
{
}


Gfx_timer::Gfx_timer(const Gfx_timer& other)
{
}


Gfx_timer::~Gfx_timer()
{
}

bool Gfx_timer::init()
{
	// Check to see if this system supports high performance timers.
	QueryPerformanceFrequency((LARGE_INTEGER*)&_frequency);
	if (_frequency == 0)
	{
		return false;
	}

	// Find out how many times the frequency counter ticks every millisecond.
	_ticks_per_ms = (float)(_frequency / 1000);

	QueryPerformanceCounter((LARGE_INTEGER*)&_start_time);

	return true;
}

void Gfx_timer::frame()
{
	INT64 current_time;
	float time_diff;

	QueryPerformanceCounter((LARGE_INTEGER*)& current_time);

    time_diff = (float)(current_time - _start_time);

	_frame_time = time_diff / _ticks_per_ms;

	_start_time = current_time;

    _total_time += _frame_time;

	return;
}

float Gfx_timer::get_time()
{
	return _frame_time / 1000.0f;
}

float Gfx_timer::get_total_time() {
    return _total_time;
}