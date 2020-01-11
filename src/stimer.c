#include "stimer.h"
#include <genesis.h>

void initTimer(Timer *_timer, u16 _delay, u16 _initTimer, void (*cal)(void))
{
    _timer->delay = _delay;
    _timer->timer = _initTimer;
    _timer->timerEvent = cal;
}

void timerUpdate(Timer *_timer)
{
    if (_timer->timer % _timer->delay == _timer->delay - 1)
    {
        _timer->timerEvent();
    }
    _timer->timer++;
}

void timerLog(Timer *_timer, u16 x, u16 y)
{
    char msg[10];
    u16 res = _timer->timer % _timer->delay;
    intToStr(res, msg, 10);
    VDP_drawText(msg, x, y);
}