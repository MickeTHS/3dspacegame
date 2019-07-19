
////////////////////////////////////////////////////////////////////////////////
// Filename: timerclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _TIMERCLASS_H_
#define _TIMERCLASS_H_


//////////////
// INCLUDES //
//////////////
#include <stdint.h>
#include <assert.h>
#include <CoreServices/CoreServices.h>
#include <mach/mach.h>
#include <mach/mach_time.h>
#include <unistd.h>


////////////////////////////////////////////////////////////////////////////////
// Class name: TimerClass
////////////////////////////////////////////////////////////////////////////////
class TimerClass
{
public:
    TimerClass();
    TimerClass(const TimerClass&);
    ~TimerClass();
    
    bool Initialize();
    void Frame();
    
    float GetTime();
    
private:
    uint64_t GetPIDTimeInNanoseconds(void);
    
    uint64_t m_frequency;
    float m_ticksPerMs;
    uint64_t m_startTime;
    float m_frameTime;
};

#endif
