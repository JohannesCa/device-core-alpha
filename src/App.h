#include "./internal/LockController.h"
#include "./internal/FeedbackBeep.h"

class App
{
private:
    LockController* lockController;
    FeedbackBeep* feedbackBeep;
public:
    App(int lockPort, int buzzerPort);
    ~App();
    void Setup();
    void OpenLock();
};