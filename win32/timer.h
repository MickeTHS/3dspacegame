#pragma once
#include <windows.h>

struct Gfx_timer
{
public:
	Gfx_timer();
	Gfx_timer(const Gfx_timer&);
	~Gfx_timer();

	bool init();
	void frame();

	float get_time();

private:
	INT64 _frequency;
	float _ticks_per_ms;
	INT64 _start_time;
	float _frame_time;
};