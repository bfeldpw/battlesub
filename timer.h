#ifndef TIMER_H
#define TIMER_H

#include <chrono>

using namespace std::chrono;
using HiResClock = high_resolution_clock;

class Timer
{
    public:
        Timer() {this->start();}
        void    start()     {Start = HiResClock::now();}
        void    stop()      {Stop = HiResClock::now();}
        void    restart()   {stop(); start();}
        double  elapsed()   {return duration<double>(Stop-Start).count();}
        double  split()     {return duration<double>(HiResClock::now() - Start).count();}
    
    private:
        
        HiResClock::time_point Start;
        HiResClock::time_point Stop;
};

#endif // TIMER_H
