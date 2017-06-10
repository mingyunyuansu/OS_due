#include "synch.h"

class AlarmClock {
    public:
        AlarmClock();
        ~AlarmClock();
        void Pause(int howLong);
        void Awaken();

    private:
        Lock * lock;
        ConditionPriority * con;
};

