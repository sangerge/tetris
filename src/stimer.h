#ifndef _STIMER_H_
#define _STIMER_H_

#include <genesis.h>

typedef struct _timer
{
    u16 delay;
    u16 timer;
    void (*timerEvent)(void);
} Timer;

void initTimer(Timer *_timer, u16 _delay, u16 _initTimer, void (*cal)(void));
void timerUpdate(Timer *_timer);
void timerLog(Timer *_timer, u16 x, u16 y);

#endif