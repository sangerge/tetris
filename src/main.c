/*
    计算位置的时候不要加偏移值
*/

#include <genesis.h>
#include <vdp.h>
#include <timer.h>
#include "stimer.h"
#include "inputEvent.h"
#include "resources.h"

#define WIDTH 10
//24行上面4行是不用显示的
#define HEIGHT 24
//地图偏移值
#define OFFSET_X 15
//4
#define OFFSET_Y 0

//绘制下一个方块位置
#define NEXT_X 8
#define NEXT_Y 8

u8 TETRIS_INIT_POS_X = WIDTH / 2 - 1;
s8 TETRIS_INIT_POS_Y = 1;
u8 FALL_DELAY = 30;

u8 map[WIDTH][HEIGHT] = {0};

Timer fallTimer;

typedef struct tetris
{
    s8 angle;
    u8 kind;
    u8 nextKind;
    Vect2D_s16 elements[4];
} Tetris;

Tetris tet;

InputEventClass p1InputClass;

u32 _line = 0;
u32 _score = 0;
u8 _combo = 0;
u8 _maxCombo = 0;
u8 _preLevel = 1;
u8 _level = 1;
//每玩够300分增加一级
u32 _addlevel = 300;
//0：logo
//1:title
//2:game
//3:intro
//4:game over
//5:info
//
s8 scene = -1;
bool gameOver = FALSE;
bool prePause = FALSE;
bool pause = FALSE;

s8 selectCurser = 0;

//OPTION选项
bool playBGM = TRUE;
//1-16
u8 bgmIndex = 1;
const u8 *curbgm = &game_bgm1[0];
const u8 *bgmList[16] = {
    game_bgm1,
    game_bgm2,
    game_bgm3,
    game_bgm4,
    game_bgm5,
    game_bgm6,
    game_bgm7,
    game_bgm8,
    game_bgm9,
    game_bgm10,
    game_bgm11,
    game_bgm12,
    game_bgm13,
    game_bgm14,
    game_bgm15,
    game_bgm16,
};

//黑夜模式，1：黑夜模式；2：白天模式
u8 colorTheme = 1;
u16 _colors[2] = {0};

void updateTetris()
{
    switch (tet.kind)
    {
    case 1:
        tet.elements[1].x = tet.elements[0].x + 1;
        tet.elements[1].y = tet.elements[0].y;
        tet.elements[2].x = tet.elements[0].x;
        tet.elements[2].y = tet.elements[0].y + 1;
        tet.elements[3].x = tet.elements[1].x;
        tet.elements[3].y = tet.elements[2].y;
        break;
    case 2:
        switch (tet.angle)
        {
        case 3:
        case 1:
            tet.elements[1].x = tet.elements[0].x - 1;
            tet.elements[1].y = tet.elements[0].y;
            tet.elements[2].x = tet.elements[0].x;
            tet.elements[2].y = tet.elements[0].y + 1;
            tet.elements[3].x = tet.elements[0].x + 1;
            tet.elements[3].y = tet.elements[0].y + 1;
            break;
        case 2:
        case 0:
            tet.elements[1].x = tet.elements[0].x;
            tet.elements[1].y = tet.elements[0].y + 1;
            tet.elements[2].x = tet.elements[0].x + 1;
            tet.elements[2].y = tet.elements[0].y;
            tet.elements[3].x = tet.elements[0].x + 1;
            tet.elements[3].y = tet.elements[0].y - 1;
            break;
        }
        break;
    case 3:
        switch (tet.angle)
        {
        case 3:
            tet.elements[1].x = tet.elements[0].x - 1;
            tet.elements[1].y = tet.elements[0].y;
            tet.elements[2].x = tet.elements[0].x + 1;
            tet.elements[2].y = tet.elements[0].y;
            tet.elements[3].x = tet.elements[0].x;
            tet.elements[3].y = tet.elements[0].y + 1;
            break;
        case 2:
            tet.elements[1].x = tet.elements[0].x;
            tet.elements[1].y = tet.elements[0].y + 1;
            tet.elements[2].x = tet.elements[0].x + 1;
            tet.elements[2].y = tet.elements[0].y;
            tet.elements[3].x = tet.elements[0].x;
            tet.elements[3].y = tet.elements[0].y - 1;
            break;
        case 1:
            tet.elements[1].x = tet.elements[0].x - 1;
            tet.elements[1].y = tet.elements[0].y;
            tet.elements[2].x = tet.elements[0].x + 1;
            tet.elements[2].y = tet.elements[0].y;
            tet.elements[3].x = tet.elements[0].x;
            tet.elements[3].y = tet.elements[0].y - 1;
            break;
        case 0:
            tet.elements[1].x = tet.elements[0].x - 1;
            tet.elements[1].y = tet.elements[0].y;
            tet.elements[2].x = tet.elements[0].x;
            tet.elements[2].y = tet.elements[0].y + 1;
            tet.elements[3].x = tet.elements[0].x;
            tet.elements[3].y = tet.elements[0].y - 1;
            break;
        }
        break;
    case 4:
        switch (tet.angle)
        {
        case 3:
        case 1:
            tet.elements[1].x = tet.elements[0].x;
            tet.elements[1].y = tet.elements[0].y - 1;
            tet.elements[2].x = tet.elements[0].x;
            tet.elements[2].y = tet.elements[0].y + 1;
            tet.elements[3].x = tet.elements[0].x;
            tet.elements[3].y = tet.elements[0].y + 2;
            break;
        case 2:
        case 0:
            tet.elements[1].x = tet.elements[0].x - 1;
            tet.elements[1].y = tet.elements[0].y;
            tet.elements[2].x = tet.elements[0].x + 1;
            tet.elements[2].y = tet.elements[0].y;
            tet.elements[3].x = tet.elements[0].x + 2;
            tet.elements[3].y = tet.elements[0].y;
            break;
        }
        break;
    case 5:
        switch (tet.angle)
        {
        case 3:
            tet.elements[1].x = tet.elements[0].x - 1;
            tet.elements[1].y = tet.elements[0].y;
            tet.elements[2].x = tet.elements[0].x + 1;
            tet.elements[2].y = tet.elements[0].y;
            tet.elements[3].x = tet.elements[0].x + 1;
            tet.elements[3].y = tet.elements[0].y + 1;
            break;
        case 2:
            tet.elements[1].x = tet.elements[0].x;
            tet.elements[1].y = tet.elements[0].y + 1;
            tet.elements[2].x = tet.elements[0].x;
            tet.elements[2].y = tet.elements[0].y - 1;
            tet.elements[3].x = tet.elements[0].x + 1;
            tet.elements[3].y = tet.elements[0].y - 1;
            break;
        case 1:
            tet.elements[1].x = tet.elements[0].x + 1;
            tet.elements[1].y = tet.elements[0].y;
            tet.elements[2].x = tet.elements[0].x - 1;
            tet.elements[2].y = tet.elements[0].y;
            tet.elements[3].x = tet.elements[0].x - 1;
            tet.elements[3].y = tet.elements[0].y - 1;
            break;
        case 0:
            tet.elements[1].x = tet.elements[0].x - 1;
            tet.elements[1].y = tet.elements[0].y + 1;
            tet.elements[2].x = tet.elements[0].x;
            tet.elements[2].y = tet.elements[0].y + 1;
            tet.elements[3].x = tet.elements[0].x;
            tet.elements[3].y = tet.elements[0].y - 1;
            break;
        }
        break;
    case 6:
        switch (tet.angle)
        {
        case 3:
            tet.elements[1].x = tet.elements[0].x - 1;
            tet.elements[1].y = tet.elements[0].y;
            tet.elements[2].x = tet.elements[0].x + 1;
            tet.elements[2].y = tet.elements[0].y;
            tet.elements[3].x = tet.elements[0].x + 1;
            tet.elements[3].y = tet.elements[0].y - 1;
            break;
        case 2:
            tet.elements[1].x = tet.elements[0].x;
            tet.elements[1].y = tet.elements[0].y + 1;
            tet.elements[2].x = tet.elements[0].x;
            tet.elements[2].y = tet.elements[0].y - 1;
            tet.elements[3].x = tet.elements[0].x - 1;
            tet.elements[3].y = tet.elements[0].y - 1;
            break;
        case 1:
            tet.elements[1].x = tet.elements[0].x + 1;
            tet.elements[1].y = tet.elements[0].y;
            tet.elements[2].x = tet.elements[0].x - 1;
            tet.elements[2].y = tet.elements[0].y;
            tet.elements[3].x = tet.elements[0].x - 1;
            tet.elements[3].y = tet.elements[0].y + 1;
            break;
        case 0:
            tet.elements[1].x = tet.elements[0].x;
            tet.elements[1].y = tet.elements[0].y - 1;
            tet.elements[2].x = tet.elements[0].x;
            tet.elements[2].y = tet.elements[0].y + 1;
            tet.elements[3].x = tet.elements[0].x + 1;
            tet.elements[3].y = tet.elements[0].y + 1;
            break;
        }
        break;
    case 7:
        switch (tet.angle)
        {
        case 3:
        case 1:
            tet.elements[1].x = tet.elements[0].x - 1;
            tet.elements[1].y = tet.elements[0].y + 1;
            tet.elements[2].x = tet.elements[0].x;
            tet.elements[2].y = tet.elements[0].y + 1;
            tet.elements[3].x = tet.elements[0].x + 1;
            tet.elements[3].y = tet.elements[0].y;
            break;
        case 2:
        case 0:
            tet.elements[1].x = tet.elements[0].x;
            tet.elements[1].y = tet.elements[0].y - 1;
            tet.elements[2].x = tet.elements[0].x + 1;
            tet.elements[2].y = tet.elements[0].y;
            tet.elements[3].x = tet.elements[0].x + 1;
            tet.elements[3].y = tet.elements[0].y + 1;
            break;
        }
        break;
    }
}

void drawNextTetris()
{
    //绘制下一个
    VDP_clearTextArea(NEXT_X, NEXT_Y, 4, 4);
    switch (tet.nextKind)
    {
    case 1:
        VDP_drawText("OO", NEXT_X, NEXT_Y);
        VDP_drawText("OO", NEXT_X, NEXT_Y + 1);
        break;
    case 2:
        VDP_drawText("OO", NEXT_X, NEXT_Y);
        VDP_drawText(" OO", NEXT_X, NEXT_Y + 1);
        break;
    case 3:
        VDP_drawText("OOO", NEXT_X, NEXT_Y);
        VDP_drawText(" O", NEXT_X, NEXT_Y + 1);
        break;
    case 4:
        VDP_drawText("O", NEXT_X, NEXT_Y);
        VDP_drawText("O", NEXT_X, NEXT_Y + 1);
        VDP_drawText("O", NEXT_X, NEXT_Y + 2);
        VDP_drawText("O", NEXT_X, NEXT_Y + 3);
        break;
    case 5:
        VDP_drawText("OOO", NEXT_X, NEXT_Y);
        VDP_drawText("  O", NEXT_X, NEXT_Y + 1);
        break;
    case 6:
        VDP_drawText("  O", NEXT_X, NEXT_Y);
        VDP_drawText("OOO", NEXT_X, NEXT_Y + 1);
        break;
    case 7:
        VDP_drawText(" OO", NEXT_X, NEXT_Y);
        VDP_drawText("OO", NEXT_X, NEXT_Y + 1);
        break;
    }
}

void generateTetris()
{
    tet.angle = 3;
    tet.kind = tet.nextKind;
    tet.nextKind = random() % 7 + 1;
    tet.elements[0].x = TETRIS_INIT_POS_X;
    tet.elements[0].y = TETRIS_INIT_POS_Y;
    updateTetris();
    drawNextTetris();
}

void fallUpdate()
{
    //resetMap();
    //是否能下落
    bool canFall = TRUE;

    //判断是否能下落
    for (int i = 0; i < 4; i++)
    {
        //y坐标超出下限
        if (tet.elements[i].y + 1 > HEIGHT - 1 ||
            //在屏幕之内并且下一个位置是8
            ((tet.elements[i].x >= 0 && tet.elements[i].x <= WIDTH - 1 && tet.elements[i].y + 1 >= 4 && tet.elements[i].y + 1 <= HEIGHT - 1) &&
             map[tet.elements[i].x][tet.elements[i].y + 1] == 8))
        {
            canFall = FALSE;
            break;
        }
    }

    if (canFall)
    {
        for (u8 i = 0; i < 4; i++)
        {
            if (tet.elements[i].x >= 0 && tet.elements[i].x <= WIDTH - 1 && tet.elements[i].y >= 4 && tet.elements[i].y <= HEIGHT - 1)
                VDP_drawText(".", OFFSET_X + tet.elements[i].x, tet.elements[i].y);
        }
        tet.elements[0].y++;
        updateTetris();
        for (u8 i = 0; i < 4; i++)
        {
            if (tet.elements[i].x >= 0 && tet.elements[i].x <= WIDTH - 1 && tet.elements[i].y >= 4 && tet.elements[i].y <= HEIGHT - 1)
                VDP_drawText("O", OFFSET_X + tet.elements[i].x, tet.elements[i].y);
        }
    }
    else
    {
        //updateTetris();
        for (u8 i = 0; i < 4; i++)
        {
            //下落之后有一个点的y坐标<4则游戏结束
            if (tet.elements[i].y < 4)
            {
                gameOver = TRUE;
                scene = 4;
                return;
            }
            //落下之后设置map值,y坐标只绘制≥4的
            if (tet.elements[i].x >= 0 && tet.elements[i].x <= WIDTH - 1 && tet.elements[i].y >= 4 && tet.elements[i].y <= HEIGHT - 1)
            {
                map[tet.elements[i].x][tet.elements[i].y] = 8;
                VDP_drawText("@", OFFSET_X + tet.elements[i].x, tet.elements[i].y);
            }
        }
        XGM_stopPlayPCM(SOUND_PCM_CH4);
        XGM_startPlayPCM(66, 0, SOUND_PCM_CH4);
        generateTetris();
    }

    //改变map数组信息
    for (int i = 0; i < 4; i++)
    {
        //在屏幕之内时
        if (tet.elements[i].x >= 0 && tet.elements[i].x <= WIDTH - 1 && tet.elements[i].y >= 4 && tet.elements[i].y <= HEIGHT - 1)
        {
            map[tet.elements[i].x][tet.elements[i].y] = tet.kind;
        }
    }

    //记录有多少行是满的
    u8 _lines = 0;
    u8 _fullLineIndexList[HEIGHT - 4] = {0};

    //计算有多少行是满的，并且标记出哪一行是满的
    for (u8 i = HEIGHT - 1; i >= 4; i--)
    {
        u8 lineNum = 0;
        for (u8 j = 0; j < WIDTH; j++)
        {
            if (map[j][i] == 8)
            {
                lineNum++;
            }
        }
        if (lineNum == WIDTH)
        {
            _lines++;
            _fullLineIndexList[i - 4] = 1;
        }
    }

    //从上往下遍历
    for (u8 i = 0; i < HEIGHT - 4; i++)
    {
        //从上往下走的
        if (_fullLineIndexList[i] == 1)
        {
            // _combo = _lines;
            for (u8 j = 0; j < WIDTH; j++)
            {
                map[j][i + 4] = 0;
                VDP_drawText(".", OFFSET_X + j, i + 4);
            }
            XGM_stopPlayPCM(SOUND_PCM_CH4);
            XGM_startPlayPCM(65, 0, SOUND_PCM_CH4);

            //当前行消除之后，当前行往上所有的行都要往下移1格
            if (i + 4 - 1 >= 4)
            {
                for (u8 m = i + 4 - 1; m >= 4; m--)
                {
                    for (u8 n = 0; n < WIDTH; n++)
                    {
                        if (map[n][m] == 8)
                        {
                            map[n][m] = 0;
                            VDP_drawText(".", OFFSET_X + n, m);
                            map[n][m + 1] = 8;
                            VDP_drawText("@", OFFSET_X + n, m + 1);
                            waitMs(10);
                        }
                    }
                }
            }
        }
    }

    //计算分数
    if (!canFall)
    {
        if (_lines > 0)
        {
            _combo += _lines;
            _line += _lines;
            _score += _lines * _lines + (_line / 2);
            if (_score > 300)
            {
                _preLevel = _level;
                _level = _score / _addlevel + 1;
                if (_level > _preLevel)
                {
                    VDP_drawText("___________", 27, 11);
                    VDP_drawText("LEVEL UP!", 28, 13);
                    VDP_drawText("___________", 27, 14);
                    XGM_stopPlayPCM(SOUND_PCM_CH4);
                    XGM_startPlayPCM(67, 0, SOUND_PCM_CH4);
                    waitMs(1500);
                    VDP_clearTextArea(27, 11, 11, 4);
                }
                fallTimer.delay = FALL_DELAY - (_level * 2);
            }
        }
        else
        {
            _combo = 0;
        }
        if (_combo > _maxCombo)
            _maxCombo = _combo;
    }
}

//设置主题
void setColorTheme()
{
    if (colorTheme == 1)
    {
        VDP_setPaletteColor(0, _colors[1]);
        VDP_setPaletteColor(15, _colors[0]);
    }
    else if (colorTheme == 2)
    {
        VDP_setPaletteColor(0, _colors[0]);
        VDP_setPaletteColor(15, _colors[1]);
    }
}

//按键Enter
void p1BtnEnterEvent(u8 key)
{
    if (scene == -1)
    {
        if (key == 0)
        {
            colorTheme--;
            if (colorTheme < 1)
                colorTheme = 2;
            XGM_stopPlayPCM(SOUND_PCM_CH4);
            XGM_startPlayPCM(64, 0, SOUND_PCM_CH4);
        }
        else if (key == 1)
        {
            colorTheme++;
            if (colorTheme > 2)
                colorTheme = 1;
            XGM_stopPlayPCM(SOUND_PCM_CH4);
            XGM_startPlayPCM(64, 0, SOUND_PCM_CH4);
        }
        else if (key == 10)
        {
            scene = 0;
        }
    }
    else if (scene == 0)
    {
        if (key == 10)
        {
            scene++;
        }
    }
    else if (scene == 1)
    {
        if (key == 0)
        {
            selectCurser--;
            XGM_stopPlayPCM(SOUND_PCM_CH4);
            XGM_startPlayPCM(64, 0, SOUND_PCM_CH4);
        }
        else if (key == 1)
        {
            selectCurser++;
            XGM_stopPlayPCM(SOUND_PCM_CH4);
            XGM_startPlayPCM(64, 0, SOUND_PCM_CH4);
        }

        {
            if (selectCurser > 1)
                selectCurser = 0;
            if (selectCurser < 0)
                selectCurser = 1;
        }

        if (key == 10)
        {
            switch (selectCurser)
            {
            case 0:
                scene = 2;
                break;
            case 1:
                scene = 3;
                break;
            }
        }
    }
    else if (scene == 2)
    {
        //暂停
        if (key == 10)
        {
            //prePause = pause;
            pause = !pause;
        }
        //关闭打开音乐
        else if (key == 7)
        {
            playBGM = !playBGM;
            if (playBGM)
            {
                VDP_drawText("BGM ON ", 17, 26);
                XGM_resumePlay();
            }
            else
            {
                VDP_drawText("BGM OFF", 17, 26);
                XGM_pausePlay();
            }
        }
        //切换主题
        else if (key == 8 && pause)
        {
            colorTheme = 1;
            setColorTheme();
        }
        else if (key == 9 && pause)
        {
            colorTheme = 2;
            setColorTheme();
        }
        //向上选择音乐
        else if (key == 0 && pause)
        {
            VDP_clearText(30, bgmIndex + 9, 1);
            bgmIndex--;
            if (bgmIndex == 0)
                bgmIndex = 16;
            curbgm = bgmList[bgmIndex - 1];
            XGM_stopPlay();
            XGM_startPlay(curbgm);
            playBGM = TRUE;
            VDP_drawText("BGM ON ", 17, 26);
            VDP_drawText(">", 30, bgmIndex + 9);
        }
        //向下选择音乐
        else if (key == 1 && pause)
        {
            VDP_clearText(30, bgmIndex + 9, 1);
            bgmIndex++;
            if (bgmIndex > 16)
                bgmIndex = 1;
            curbgm = bgmList[bgmIndex - 1];
            XGM_stopPlay();
            XGM_startPlay(curbgm);
            playBGM = TRUE;
            VDP_drawText("BGM ON ", 17, 26);
            VDP_drawText(">", 30, bgmIndex + 9);
        }
        if (pause)
            return;
        //左移
        if (key == 2)
        {
            u8 min_x_index = 0;
            for (u8 i = 1; i < 4; i++)
            {
                if (tet.elements[min_x_index].x > tet.elements[i].x)
                {
                    min_x_index = i;
                }
            }
            //左边没有超出边界
            if (tet.elements[min_x_index].x > 0)
            {
                //下一个位置是否是0，先移动，后再判断
                {
                    //清除上一个位置的显示
                    for (u8 i = 0; i < 4; i++)
                    {
                        // && tet.elements[i].y >= 0 && tet.elements[i].y <= HEIGHT - 1
                        if (tet.elements[i].x >= 0 && tet.elements[i].x <= WIDTH - 1 && tet.elements[i].y >= 4 && tet.elements[i].y <= HEIGHT - 1)
                            VDP_drawText(".", OFFSET_X + tet.elements[i].x, tet.elements[i].y);
                    }
                }
                tet.elements[0].x--;
                updateTetris();
                //往回走
                for (u8 i = 0; i < 4; i++)
                {
                    if (map[tet.elements[i].x][tet.elements[i].y] == 8)
                    {
                        tet.elements[0].x++;
                        updateTetris();
                        break;
                    }
                }
                for (u8 i = 0; i < 4; i++)
                {
                    if (tet.elements[i].x >= 0 && tet.elements[i].x <= WIDTH - 1 && tet.elements[i].y >= 4 && tet.elements[i].y <= HEIGHT - 1)
                        VDP_drawText("O", OFFSET_X + tet.elements[i].x, tet.elements[i].y);
                }
            }
        }
        //右移
        else if (key == 3)
        {
            //是否能左右移动，找出x最大的点的索引
            u8 max_x_index = 0;
            for (u8 i = 1; i < 4; i++)
            {
                if (tet.elements[max_x_index].x < tet.elements[i].x)
                {
                    max_x_index = i;
                }
            }
            if (tet.elements[max_x_index].x < WIDTH - 1)
            {
                //下一个位置是否是0，先移动，后再判断
                for (u8 i = 0; i < 4; i++)
                {
                    if (tet.elements[i].x >= 0 && tet.elements[i].x <= WIDTH - 1 && tet.elements[i].y >= 4 && tet.elements[i].y <= HEIGHT - 1)
                        VDP_drawText(".", OFFSET_X + tet.elements[i].x, tet.elements[i].y);
                }

                tet.elements[0].x++;
                updateTetris();
                for (u8 i = 0; i < 4; i++)
                {
                    if (map[tet.elements[i].x][tet.elements[i].y] == 8)
                    {
                        tet.elements[0].x--;
                        updateTetris();
                        break;
                    }
                }
                for (u8 i = 0; i < 4; i++)
                {
                    if (tet.elements[i].x >= 0 && tet.elements[i].x <= WIDTH - 1 && tet.elements[i].y >= 4 && tet.elements[i].y <= HEIGHT - 1)
                        VDP_drawText("O", OFFSET_X + tet.elements[i].x, tet.elements[i].y);
                }
            }
        }

        //A键左转
        if (key == 4)
        {
            //清除上一个位置的显示
            for (u8 i = 0; i < 4; i++)
            {
                if (tet.elements[i].x >= 0 && tet.elements[i].x <= WIDTH - 1 && tet.elements[i].y >= 4 && tet.elements[i].y <= HEIGHT - 1)
                    VDP_drawText(".", OFFSET_X + tet.elements[i].x, tet.elements[i].y);
            }
            tet.angle--;
            if (tet.angle < 0)
                tet.angle = 3;
            if (tet.angle > 3)
                tet.angle = 0;
            updateTetris();
            //如果旋转后碰到8，回转
            for (u8 i = 0; i < 4; i++)
            {
                if (map[tet.elements[i].x][tet.elements[i].y] == 8)
                {
                    tet.angle++;
                    updateTetris();
                    break;
                }
            }
            u8 min_x_index = 0;
            u8 max_x_index = 0;
            for (u8 i = 1; i < 4; i++)
            {
                if (tet.elements[i].x < tet.elements[min_x_index].x)
                {
                    min_x_index = i;
                }
                if (tet.elements[i].x > tet.elements[max_x_index].x)
                {
                    max_x_index = i;
                }
            }

            //计算超出多少
            u8 _tmp = 0;
            if (tet.elements[min_x_index].x < 0)
            {
                _tmp = 0 - tet.elements[min_x_index].x;
                tet.elements[0].x += _tmp;
                updateTetris();
            }
            else if (tet.elements[max_x_index].x > WIDTH - 1)
            {
                _tmp = tet.elements[max_x_index].x - (WIDTH - 1);
                tet.elements[0].x -= _tmp;
                updateTetris();
            }

            for (u8 i = 0; i < 4; i++)
            {
                if (tet.elements[i].x >= 0 && tet.elements[i].x <= WIDTH - 1 && tet.elements[i].y >= 4 && tet.elements[i].y <= HEIGHT - 1)
                    VDP_drawText("O", OFFSET_X + tet.elements[i].x, OFFSET_Y + tet.elements[i].y);
            }
        }
        //B键右转
        else if (key == 5)
        {
            //清除上一个位置的显示
            for (u8 i = 0; i < 4; i++)
            {
                if (tet.elements[i].x >= 0 && tet.elements[i].x <= WIDTH - 1 && tet.elements[i].y >= 4 && tet.elements[i].y <= HEIGHT - 1)
                    VDP_drawText(".", OFFSET_X + tet.elements[i].x, OFFSET_Y + tet.elements[i].y);
            }

            tet.angle++;
            if (tet.angle < 0)
                tet.angle = 3;
            if (tet.angle > 3)
                tet.angle = 0;
            updateTetris();

            for (u8 i = 0; i < 4; i++)
            {
                if (map[tet.elements[i].x][tet.elements[i].y] == 8)
                {
                    tet.angle--;
                    updateTetris();
                    break;
                }
            }
            u8 min_x_index = 0;
            u8 max_x_index = 0;
            for (int i = 1; i < 4; i++)
            {
                if (tet.elements[i].x < tet.elements[min_x_index].x)
                {
                    min_x_index = i;
                }
                if (tet.elements[i].x > tet.elements[max_x_index].x)
                {
                    max_x_index = i;
                }
            }
            u8 _tmp = 0;
            if (tet.elements[min_x_index].x < 0)
            {
                //计算超出多少
                _tmp = 0 - tet.elements[min_x_index].x;
                tet.elements[0].x += _tmp;
                updateTetris();
            }
            else if (tet.elements[max_x_index].x > WIDTH - 1)
            {
                _tmp = tet.elements[max_x_index].x - (WIDTH - 1);
                tet.elements[0].x -= _tmp;
                updateTetris();
            }
            //showMap();
            for (u8 i = 0; i < 4; i++)
            {
                if (tet.elements[i].x >= 0 && tet.elements[i].x <= WIDTH - 1 && tet.elements[i].y >= 4 && tet.elements[i].y <= HEIGHT - 1)
                    VDP_drawText("O", OFFSET_X + tet.elements[i].x, OFFSET_Y + tet.elements[i].y);
            }
        }
        //快速
        else if (key == 6)
        {
            fallTimer.delay = 1;
        }
    }
    else if (scene == 3)
    {
        if (key == 10)
        {
            scene = 1;
        }
    }
    else if (scene == 5)
    {
        if (key == 10)
        {
            scene = 0;
        }
    }
}

void p1BtnExitEvent(u8 key)
{
    if (scene == 2 && key == 6)
    {
        fallTimer.delay = FALL_DELAY - (_level * 2);
    }
}

int main()
{
    VDP_setPalette(PAL0, info.palette->data);
    TETRIS_INIT_POS_X = WIDTH / 2 - 1;
    initInputEventClass(&p1InputClass, JOY_1);
    regButtonEnterEvent(&p1InputClass, p1BtnEnterEvent);
    regButtonExitEvent(&p1InputClass, p1BtnExitEvent);

    memcpyU16(_colors, info.palette->data, 2);
    char msg_score[10];
    char msg_line[10];
    char msg_combo[3];
    char msg_maxCombo[3];
    char msg_level[3];

    XGM_setPCM(64, ding, sizeof(ding));
    XGM_setPCM(65, clear, sizeof(clear));
    XGM_setPCM(66, duang, sizeof(duang));
    XGM_setPCM(67, level_up, sizeof(level_up));

    while (TRUE)
    {

        //颜色选择界面
        XGM_startPlay(colortheme_bgm);

        while (scene == -1)
        {
            inputProcessUpdate(&p1InputClass);

            {
                VDP_drawText("Select Color Theme", 12, 6);
                VDP_drawText("Dark", 19, 15);
                VDP_drawText("Light", 19, 17);
            }
            if (colorTheme == 1)
            {
                VDP_setPaletteColor(0, _colors[1]);
                VDP_setPaletteColor(15, _colors[0]);

                VDP_clearText(18, 17, 1);
                VDP_drawText(">", 18, 15);
            }
            else if (colorTheme == 2)
            {
                VDP_setPaletteColor(0, _colors[0]);
                VDP_setPaletteColor(15, _colors[1]);

                VDP_clearText(18, 15, 1);
                VDP_drawText(">", 18, 17);
            }
            if (scene != -1)
            {
                VDP_clearTextArea(0, 0, 40, 28);
                XGM_stopPlay();
            }
        }

        //LOGO界面
        setColorTheme();
        Z80_startReset();
        Z80_endReset();
        XGM_startPlay(logo_bgm);
        while (scene == 0)
        {
            VDP_drawText("SEGA", 19, 12);
            VDP_drawText("press start button", 12, 18);
            inputProcessUpdate(&p1InputClass);
            if (scene != 0)
            {
                VDP_clearTextArea(0, 0, 40, 28);
                XGM_stopPlay();
            }
        }

        //Title界面
        XGM_startPlay(title_bgm);
        if (scene == 1)
        {
            _maxCombo = 0;
            VDP_drawText("_________________", 11, 2);
            VDP_drawText("Console  Tetris", 12, 4);
            VDP_drawText("_________________", 11, 5);

            {
                VDP_drawText("=================", 11, 9);
                VDP_drawText("=================", 11, 15);
                for (int i = 10; i < 15; i++)
                {
                    VDP_drawText("|", 11, i);
                    VDP_drawText("|", 27, i);
                }
            }

            {
                VDP_drawText("OOO|OO|OO |OOOO", 12, 10);
                VDP_drawText("  O|OO| OO|", 12, 11);
                VDP_drawText("_______________", 12, 12);
                VDP_drawText(" OOO | OO|   O", 12, 13);
                VDP_drawText("  O  |OO | OOO", 12, 14);
            }

            {
                VDP_drawText("Play", 18, 18);
                VDP_drawText("Intro", 18, 20);
                VDP_drawText("Retro Game Dev - 2020", 10, 23);
            }
        }
        while (scene == 1)
        {
            inputProcessUpdate(&p1InputClass);
            {
                //绘制光标
                switch (selectCurser)
                {
                case 0:
                    VDP_clearTextArea(17, 20, 1, 1);
                    VDP_drawText(">", 17, 18);
                    break;
                case 1:
                    VDP_clearTextArea(17, 18, 1, 1);
                    VDP_drawText(">", 17, 20);
                    break;
                }
            }
            if (scene != 1)
            {
                VDP_clearTextArea(0, 0, 40, 28);
                XGM_stopPlay();
            }
        }

        //游戏界面
        while (scene == 2)
        {
            VDP_clearTextArea(0, 0, 40, 28);
            //绘制空地图
            for (u8 i = 4; i < HEIGHT; i++)
            {
                for (u8 j = 0; j < WIDTH; j++)
                {
                    map[j][i] = 0;
                    VDP_drawText(".", OFFSET_X + j, i);
                }
            }
            //初始化数据
            {
                gameOver = FALSE;
                tet.angle = 3;
                tet.nextKind = random() % 7 + 1;
                _line = 0;
                _score = 0;
                _combo = 0;
                _level = 1;
                _maxCombo = 0;
                initTimer(&fallTimer, FALL_DELAY, FALL_DELAY - 1, fallUpdate);
                fallTimer.delay = FALL_DELAY - (_level * 2);
                generateTetris();
            }

            //静态UI
            {
                {
                    //绘制文字横边框
                    for (u8 i = 0; i < 11; i++)
                    {
                        VDP_drawText("_", 2 + i, 1);
                    }
                    for (u8 i = 0; i < 11; i++)
                    {
                        VDP_drawText("_", 2 + i, 5);
                    }
                    for (u8 i = 0; i < 11; i++)
                    {
                        VDP_drawText("_", 2 + i, 12);
                    }
                    for (u8 i = 0; i < 11; i++)
                    {
                        VDP_drawText("_", 2 + i, 15);
                    }
                    for (u8 i = 0; i < 11; i++)
                    {
                        VDP_drawText("_", 2 + i, 18);
                    }
                    for (u8 i = 0; i < 11; i++)
                    {
                        VDP_drawText("_", 2 + i, 21);
                    }
                    for (u8 i = 0; i < 11; i++)
                    {
                        VDP_drawText("_", 2 + i, 26);
                    }

                    //竖边框
                    for (u8 i = 0; i < 38; i++)
                    {
                        VDP_drawText("|", 1, 1 + i);
                    }
                    for (u8 i = 0; i < 38; i++)
                    {
                        VDP_drawText("|", 13, 1 + i);
                    }
                }
                //分数
                VDP_drawText("SCORE:", 2, 2);
                //下一个方块
                VDP_drawText("NEXT:", 2, 6);
                //连击
                VDP_drawText("COMBO:", 2, 13);
                //消除的行数
                VDP_drawText("LINE:", 2, 16);
                //绘制最大连击数
                VDP_drawText("MAX COMBO:", 2, 19);
                //绘制级别
                VDP_drawText("LEVEL:", 2, 22);

                {
                    //界面窗体边框
                    for (u8 i = 0; i < 40; i++)
                    {
                        VDP_drawText("_", i, 0);
                    }
                    for (u8 i = 0; i < 40; i++)
                    {
                        VDP_drawText("_", i, 27);
                    }
                    for (u8 i = 0; i < 28; i++)
                    {
                        VDP_drawText("|", 0, i);
                    }
                    for (u8 i = 0; i < 28; i++)
                    {
                        VDP_drawText("|", 39, i);
                    }
                    //方块主窗体边框
                    for (u8 i = 0; i <= WIDTH + 1; i++)
                    {
                        VDP_drawText("_", OFFSET_X + i - 1, 4 - 1);
                    }
                    for (u8 i = 0; i <= WIDTH + 1; i++)
                    {
                        VDP_drawText("_", OFFSET_X + i - 1, HEIGHT);
                    }
                    for (u8 i = 4; i < HEIGHT; i++)
                    {
                        VDP_drawText("|", OFFSET_X - 1, i);
                    }
                    for (u8 i = 4; i < HEIGHT; i++)
                    {
                        VDP_drawText("|", OFFSET_X + WIDTH, i);
                    }
                }
            }

            VDP_drawText("BGM ON ", 17, 26);
            //播放音乐
            XGM_startPlay(curbgm);

            while (gameOver == FALSE)
            {
                prePause = pause;
                //动态UI
                {
                    //绘制分数
                    //char msg_score[10];
                    intToStr(_score, msg_score, 10);
                    VDP_drawText(msg_score, 3, 3);
                    //连击数
                    //char msg_combo[3];
                    intToStr(_combo, msg_combo, 3);
                    VDP_drawText(msg_combo, 10, 14);
                    //消除行数
                    //char msg_line[10];
                    intToStr(_line, msg_line, 10);
                    VDP_drawText(msg_line, 3, 17);
                    //最大连击数
                    //char msg_maxCombo[3];
                    intToStr(_maxCombo, msg_maxCombo, 3);
                    VDP_drawText(msg_maxCombo, 10, 20);
                    //级别
                    //char msg_level[3];
                    intToStr(_level, msg_level, 3);
                    VDP_drawText(msg_level, 10, 23);
                }

                //显示FPS
                // VDP_showFPS(FALSE);

                inputProcessUpdate(&p1InputClass);
                //游戏中option菜单X坐标
                u8 _optionX = OFFSET_X + WIDTH + 2;

                //运行中
                if (!pause && !prePause)
                {
                    timerUpdate(&fallTimer);
                }
                //恢复游戏时
                else if (!pause && prePause)
                {
                    //清除PAUSED显示
                    VDP_clearTextArea(17, 2, 6, 1);
                    //清除[Menu]
                    VDP_clearTextArea(_optionX - 1, 1, 40 - _optionX, 26);
                }
                //暂停游戏时
                else if (pause && !prePause)
                {
                    { //绘制边框
                        for (u8 i = 0; i < 13; i++)
                        {
                            VDP_drawText("_", _optionX + i - 1, 1);
                        }
                        for (u8 i = 0; i < 13; i++)
                        {
                            VDP_drawText("_", _optionX + i - 1, 26);
                        }
                        for (u8 i = 0; i < 26; i++)
                        {
                            VDP_drawText("|", _optionX - 1, 1 + i);
                        }
                        for (u8 i = 0; i < 26; i++)
                        {
                            VDP_drawText("|", 38, 1 + i);
                        }
                    }
                    //绘制暂停菜单
                    VDP_drawText("PAUSED", 17, 2);
                    //绘制游戏中option
                    VDP_drawText("[Menu]", _optionX + 3, 2);
                    VDP_drawText("Theme:", _optionX, 4);
                    VDP_drawText("Y:Dark", _optionX + 4, 4 + 1);
                    VDP_drawText("Z:Light", _optionX + 4, 4 + 2);

                    VDP_drawText("BGM:", _optionX, 4 + 4);
                    VDP_drawText(">", 30, bgmIndex + 9);
                    VDP_drawText("BGM_01", _optionX + 4, 4 + 6);
                    VDP_drawText("BGM_02", _optionX + 4, 4 + 7);
                    VDP_drawText("BGM_03", _optionX + 4, 4 + 8);
                    VDP_drawText("BGM_04", _optionX + 4, 4 + 9);
                    VDP_drawText("BGM_05", _optionX + 4, 4 + 10);
                    VDP_drawText("BGM_06", _optionX + 4, 4 + 11);
                    VDP_drawText("BGM_07", _optionX + 4, 4 + 12);
                    VDP_drawText("BGM_08", _optionX + 4, 4 + 13);
                    VDP_drawText("BGM_09", _optionX + 4, 4 + 14);
                    VDP_drawText("BGM_10", _optionX + 4, 4 + 15);
                    VDP_drawText("BGM_11", _optionX + 4, 4 + 16);
                    VDP_drawText("BGM_12", _optionX + 4, 4 + 17);
                    VDP_drawText("BGM_13", _optionX + 4, 4 + 18);
                    VDP_drawText("BGM_14", _optionX + 4, 4 + 19);
                    VDP_drawText("BGM_15", _optionX + 4, 4 + 20);
                    VDP_drawText("BGM_16", _optionX + 4, 4 + 21);
                }

                if (gameOver)
                {
                    XGM_stopPlay();

                    waitMs(500);
                    VDP_drawText("GAME OVER!", 15, 2);
                    XGM_stopPlay();

                    XGM_startPlay(gameover_sound);
                    waitMs(7000);
                }
                VDP_waitVSync();
            }
            if (scene != 2)
            {
                VDP_clearTextArea(0, 0, 40, 28);
                XGM_stopPlay();
            }
        }

        //intro scene
        if (scene == 3)
        {
            //显示intro
            if (colorTheme == 1)
            {
                VDP_setPaletteColor(0, _colors[1]);
                VDP_setPaletteColor(1, _colors[0]);
            }
            else
            {
                VDP_setPaletteColor(0, _colors[0]);
                VDP_setPaletteColor(1, _colors[1]);
            }

            VDP_drawImageEx(PLAN_B, &intro, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, TILE_USERINDEX), 0, 0, FALSE, DMA);
            Z80_startReset();
            Z80_endReset();
            XGM_startPlay(intro_bgm);
            waitMs(1000);
            VDP_drawText("press start button to exit", 7, 27);
        }

        while (scene == 3)
        {
            inputProcessUpdate(&p1InputClass);
            if (scene != 3)
            {
                VDP_clearPlan(PLAN_B, TRUE);
                VDP_clearTextArea(0, 0, 40, 28);
                XGM_stopPlay();
            }
            VDP_waitVSync();
        }

        if (scene == 4)
        {
            VDP_drawText("____________________", 10, 4);
            VDP_drawText("GAME OVER", 11, 6);
            VDP_drawText("____________________", 10, 7);
            VDP_drawText("____________________", 10, 9);
            VDP_drawText(">SCORE:", 11, 11);
            VDP_drawText(">LINE:", 11, 15);
            VDP_drawText(">MAX COMBO:", 11, 19);
            VDP_drawText(">LEVEL:", 11, 23);

            intToStr(_score, msg_score, 10);
            VDP_drawText(msg_score, 12, 12);

            intToStr(_line, msg_line, 10);
            VDP_drawText(msg_line, 12, 16);

            intToStr(_maxCombo, msg_maxCombo, 3);
            VDP_drawText(msg_maxCombo, 12, 20);

            intToStr(_level, msg_level, 3);
            VDP_drawText(msg_level, 12, 24);
            VDP_drawText("____________________", 10, 25);

            XGM_startPlay(gameover_bgm);
        }
        //游戏结束
        while (scene == 4)
        {
            waitMs(10000);
            scene = 5;
            if (scene != 4)
            {
                VDP_clearTextArea(0, 0, 40, 28);
                XGM_stopPlay();
            }
        }

        if (scene == 5)
        {
            //显示制作信息
            if (colorTheme == 1)
            {
                VDP_setPaletteColor(0, _colors[1]);
                VDP_setPaletteColor(1, _colors[0]);
            }
            else
            {
                VDP_setPaletteColor(0, _colors[0]);
                VDP_setPaletteColor(1, _colors[1]);
            }

            VDP_drawImageEx(PLAN_B, &info, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, TILE_USERINDEX), 0, 0, FALSE, DMA);
            //等待几秒钟
            waitMs(4000);
            VDP_drawText("press start button", 11, 21);
        }

        while (scene == 5)
        {
            inputProcessUpdate(&p1InputClass);
            if (scene != 5)
            {
                VDP_clearPlan(PLAN_B, TRUE);
                VDP_clearTextArea(0, 0, 40, 28);
                XGM_stopPlay();
            }
            VDP_waitVSync();
        }
    }

    return 0;
}