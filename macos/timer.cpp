///////////////////////////////////////////////////////////////////////////////
// Filename: timerclass.cpp
///////////////////////////////////////////////////////////////////////////////
#include "timer.h"


TimerClass::TimerClass()
{
}


TimerClass::TimerClass(const TimerClass& other)
{
}


TimerClass::~TimerClass()
{
}

bool TimerClass::Initialize()
{
    // Check to see if this system supports high performance timers.
    // Find out how many times the frequency counter ticks every millisecond.
    //m_ticksPerMs = (float)(m_frequency / 1000);
    
    m_startTime = GetPIDTimeInNanoseconds();
    
    return true;
}

void TimerClass::Frame()
{
    m_frameTime = GetPIDTimeInNanoseconds() / 10.0f;
}

float TimerClass::GetTime()
{
    return m_frameTime;
}

uint64_t TimerClass::GetPIDTimeInNanoseconds(void)
{
    uint64_t        start;
    uint64_t        end;
    uint64_t        elapsed;
    
    // Start the clock.
    
    start = mach_absolute_time();
    
    // Call getpid. This will produce inaccurate results because
    // we're only making a single system call. For more accurate
    // results you should call getpid multiple times and average
    // the results.
    
    (void) getpid();
    
    // Stop the clock.
    
    end = mach_absolute_time();
    
    // Calculate the duration.
    
    elapsed = end - start;
    
    // Convert to nanoseconds.
    
    // Have to do some pointer fun because AbsoluteToNanoseconds
    // works in terms of UnsignedWide, which is a structure rather
    // than a proper 64-bit integer.
    
    
    
    return elapsed;
}

