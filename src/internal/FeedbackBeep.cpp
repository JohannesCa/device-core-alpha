#include <Arduino.h>
#include "FeedbackBeep.h"

FeedbackBeep::FeedbackBeep(int buzzer)
{
    _buzzerPort = buzzer;
}

void FeedbackBeep::Setup()
{
    pinMode(_buzzerPort, OUTPUT);

    // tone(_buzzerPort, Notes.A, 500);
    // delay(1000);

    int wait = 100;
    tone(_buzzerPort, Notes.Fsus, wait);
    delay(wait);
    // tone(_buzzerPort, Notes.Gsus, wait);
    // delay(wait);
    tone(_buzzerPort, Notes.A, wait);
}

void FeedbackBeep::OpenBeep()
{
    tone(_buzzerPort, Notes.A, 150);
    delay(150);
    // tone(_buzzerPort, Notes.A, 100);
}

void FeedbackBeep::CloseBeep()
{
    tone(_buzzerPort, Notes.Gsus, 600);
    delay(600);
}