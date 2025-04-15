#ifndef FEEDBABCK_BEEP_H
#define FEEDBABCK_BEEP_H

struct {
    int Fsus = 739;
    int Gsus = 784;
    int A    = 880;
} Notes;

class FeedbackBeep
{
private:
    int _buzzerPort;
public:
    FeedbackBeep(int buzzer);
    void Setup();
    void OpenBeep();
    void CloseBeep();
};

#endif // FEEDBABCK_BEEP_H