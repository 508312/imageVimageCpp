#ifndef TIMER_H
#define TIMER_H
#include <chrono>

/** Timer class mostly for debug and stats **/
class Timer
{
    public:
        Timer();
        virtual ~Timer();
        /** Starts timer. **/
        void start();

        /** Gets time in microseconds between now and last start() command **/
        int get();

    protected:

    private:
        std::chrono::time_point<std::chrono::steady_clock> startT;
};

#endif // TIMER_H
