#include <genesis.h>
#include "inputEvent.h"

static void everyButton(InputEventClass *_class, u16 BUTTON_, u8 index_);
static void everyEvent(InputEventClass *_class, u8 key);

void initInputEventClass(InputEventClass *_class, u16 _JOY)
{
    _class->JOY = _JOY;
    _class->inputValue = 0;
    for (int i = 0; i < 12; i++)
    {
        _class->preInputList[i] = 0;
        _class->curInputList[i] = 0;
    }

    _class->buttonEnter = NULL;
    _class->buttonStay = NULL;
    _class->buttonExit = NULL;
}

void inputProcessUpdate(InputEventClass *_class)
{
    {
        _class->inputValue = JOY_readJoypad(_class->JOY);

        everyButton(_class, BUTTON_UP, 0);
        everyButton(_class, BUTTON_DOWN, 1);
        everyButton(_class, BUTTON_LEFT, 2);
        everyButton(_class, BUTTON_RIGHT, 3);
        everyButton(_class, BUTTON_A, 4);
        everyButton(_class, BUTTON_B, 5);
        everyButton(_class, BUTTON_C, 6);
        everyButton(_class, BUTTON_X, 7);
        everyButton(_class, BUTTON_Y, 8);
        everyButton(_class, BUTTON_Z, 9);
        everyButton(_class, BUTTON_START, 10);
        everyButton(_class, BUTTON_MODE, 11);
    }

    {
        everyEvent(_class, 0);
        everyEvent(_class, 1);
        everyEvent(_class, 2);
        everyEvent(_class, 3);
        everyEvent(_class, 4);
        everyEvent(_class, 5);
        everyEvent(_class, 6);
        everyEvent(_class, 7);
        everyEvent(_class, 8);
        everyEvent(_class, 9);
        everyEvent(_class, 10);
        everyEvent(_class, 11);
    }
}

void regButtonEnterEvent(InputEventClass *_class, void (*cal)(u8 key))
{
    _class->buttonEnter = cal;
}

void regButtonExitEvent(InputEventClass *_class, void (*cal)(u8 key))
{
    _class->buttonExit = cal;
}

void regButtonStayEvent(InputEventClass *_class, void (*cal)(u8 key))
{
    _class->buttonStay = cal;
}

static void everyButton(InputEventClass *_class, u16 BUTTON_, u8 index_)
{
    _class->preInputList[index_] = _class->curInputList[index_];

    if (_class->inputValue & BUTTON_)
    {
        _class->curInputList[index_] = 1;
    }
    else
    {
        _class->curInputList[index_] = 0;
    }
}

static void everyEvent(InputEventClass *_class, u8 key)
{
    if (_class->curInputList[key] != _class->preInputList[key])
    {
        if (_class->buttonEnter && _class->curInputList[key])
        {
            _class->buttonEnter(key);
        }
        else if (_class->buttonExit)
        {
            _class->buttonExit(key);
        }
    }
    else if (_class->buttonStay && _class->preInputList[key] && _class->curInputList[key])
    {
        _class->buttonStay(key);
    }
}
