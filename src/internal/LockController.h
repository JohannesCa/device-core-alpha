#ifndef LOCK_CONTROLLER_H
#define LOCK_CONTROLLER_H

#include "FeedbackBeep.h"

class LockController
{
private:
    int _lockPort;
    int _defaultWait;
public:
    LockController(int lock);
    void Setup();
    void Open(int waitSec, FeedbackBeep* _buzzer);
};

#endif //LOCK_CONTROLLER_H
