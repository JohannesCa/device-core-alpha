#include <Arduino.h>
#include "LockController.h"
#include "FeedbackBeep.h"

LockController::LockController(int lock)
{
    _lockPort = lock;
    _defaultWait = 10000;
}

void LockController::Setup()
{
    pinMode(_lockPort, OUTPUT);
    digitalWrite(_lockPort, LOW);
}

void LockController::Open(int waitSec, FeedbackBeep* _buzzer)
{
    digitalWrite(_lockPort, HIGH);

    _buzzer->OpenBeep();

    int wait;
    if (waitSec <= 0) {
        wait = _defaultWait;
    } else {
        wait = waitSec*1000;
    }

    delay(wait);
    digitalWrite(_lockPort, LOW);

    _buzzer->CloseBeep();
}