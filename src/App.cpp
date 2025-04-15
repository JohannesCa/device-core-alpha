#include "App.h"

App::App(int lockPort, int buzzerPort)
{
    lockController = new LockController(lockPort);
    feedbackBeep = new FeedbackBeep(buzzerPort);
}

App::~App()
{
    delete lockController;
    delete feedbackBeep;
}

void App::Setup()
{
    lockController->Setup();
    feedbackBeep->Setup();
}

void App::OpenLock() {
    lockController->Open(5, feedbackBeep);
}