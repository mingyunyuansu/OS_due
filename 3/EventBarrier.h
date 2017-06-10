#include "synch.h"

#define SIGNALED 1
#define UNSIGNALED 0

class EventBarrier {
    public:
        EventBarrier();
        ~EventBarrier();

        void Wait();
        void Signal();
        void Complete();
        int Waiters();
        int return_waiter_num();
    private:
        Lock * lock;
        Condition * con_signal;
        Condition *con_complete;
        Condition * con_wait;
        int status;
        int waiter_num;
};

