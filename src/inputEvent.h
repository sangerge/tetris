#ifndef _INPUTEVENT_H_
#define _INPUTEVENT_H_

typedef struct inputEventClass
{
    //手柄端口，比如1p手柄为JOY_1，2p手柄为JOY_2，详见joy.h
    u16 JOY;
    //用来获取键值，这个变量不能私自改变
    u16 inputValue;
    /* 
    这俩数组存储每个按键是否按下，1为按下，0为没有按下，一共有12个按键，对应索引0-11
    0  - UP
    1  - DOWN
    2  - LEFT
    3  - RIGHT
    4  - A
    5  - B
    6  - C
    7  - X
    8  - Y
    9  - Z
    10 - START
    11 - MODE
*/
    u8 preInputList[12];
    u8 curInputList[12];

    //按键进入事件
    void (*buttonEnter)(u8 key);

    //按键持续事件
    void (*buttonStay)(u8 key);

    //按键离开事件
    void (*buttonExit)(u8 key);

} InputEventClass;

//初始化
void initInputEventClass(InputEventClass *_class, u16 _JOY);

//每一帧调用输入处理
void inputProcessUpdate(InputEventClass *_class);

//注册按键进入事件
void regButtonEnterEvent(InputEventClass *_class, void (*cal)(u8 key));

//注册按键离开事件
void regButtonExitEvent(InputEventClass *_class, void (*cal)(u8 key));

//注册按键持续事件
void regButtonStayEvent(InputEventClass *_class, void (*cal)(u8 key));

#endif 