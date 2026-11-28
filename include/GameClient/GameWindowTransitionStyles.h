// Native style layouts follow Transition's proved10-byte prefix.
// ScoreScaleUp parser35D6F9 allocates34; its init proves center24/step2C.
// FullFade ctor35F2B3/update35F241 prove start10/end14/pos18/size20/percent28/state2C.
// CountUp parser35F6E2 allocates40; ctor35F64A/dtor35F694 prove the two strings.
#pragma once
#include "GameClient/GameWindowTransitions.h"
#include "Common/ICoord2D.h"
#include "Common/UnicodeString.h"
// ReverseSound: no members beyond Transition (retail 0x35D4DB init, 0x35D4E6 update: frames 0..2, 0x35D512 skip).
class ReverseSoundTransition : public Transition {
public:
    ReverseSoundTransition();
    virtual ~ReverseSoundTransition();
    virtual void init(GameWindow *);       // 0x0035D4DB
    virtual void update(int);
    virtual void reverse();
    virtual void draw() {}
    virtual void skip();                   // 0x0035D512
};
class ScoreScaleUpTransition : public Transition {
public:
    ScoreScaleUpTransition();
    virtual ~ScoreScaleUpTransition();
    virtual void init(GameWindow *);
    virtual void update(int);
    virtual void reverse();
    virtual void draw();
    virtual void skip();
protected:
    ICoord2D m_pos,m_size; //10,18
    int m_drawState; //20
    ICoord2D m_center,m_incrementSize; //24,2C
};
class FullFadeTransition : public Transition {
public:
    FullFadeTransition();
    virtual ~FullFadeTransition();
    virtual void init(GameWindow *);
    virtual void update(int);
    virtual void reverse();
    virtual void draw();
    virtual void skip();
protected:
    int m_startFrame,m_endFrame; //10,14
    ICoord2D m_pos,m_size; //18,20
    float m_percent; //28
    int m_drawState; //2C
};
class CountUpTransition : public Transition {
public:
    CountUpTransition();
    virtual ~CountUpTransition();
    virtual void init(GameWindow *);
    virtual void update(int);
    virtual void reverse();
    virtual void draw() {}
    virtual void skip();
protected:
    int m_startFrame,m_endFrame; //10,14
    ICoord2D m_pos,m_size; //18,20
    int m_drawState; //28
    UnicodeString m_fullText,m_partialText; //2C,30
    int m_intValue,m_currentValue,m_countState; //34,38,3C
};
typedef char ScoreScaleUpTransitionSize[sizeof(ScoreScaleUpTransition)==0x34?1:-1];
typedef char FullFadeTransitionSize[sizeof(FullFadeTransition)==0x30?1:-1];
typedef char CountUpTransitionSize[sizeof(CountUpTransition)==0x40?1:-1];
// Arrow factory35D7ED allocates38; update35D75D uses state30.
class Image;
class ControlBarArrowTransition : public Transition {
public:
    virtual ~ControlBarArrowTransition();
    virtual void init(GameWindow *);
    virtual void update(int);
    virtual void reverse();
    virtual void draw();
    virtual void skip();
protected:
    ICoord2D m_pos,m_incrementPos,m_size; //10,18,20
    float m_percent,m_fadePercent; //28,2C
    int m_drawState; //30
    const Image *m_arrowImage; //34
};
// Small-scale init35DBF3 and update35DA10 establish the grow-window layout.
class MainMenuSmallScaleDownTransition : public Transition {
public:
    virtual ~MainMenuSmallScaleDownTransition();
    virtual void init(GameWindow *);
    virtual void update(int);
    virtual void reverse();
    virtual void draw();
    virtual void skip();
protected:
    ICoord2D m_pos,m_size; //10,18
    int m_drawState; //20
    ICoord2D m_growPos,m_growSize,m_incrementSize; //24,2C,34
    GameWindow *m_growWin; //3C
};
// Medium ctor35DE55/init35DF18/update35DD08 establish this48-byte variant.
class MainMenuMediumScaleUpTransition : public Transition {
public:
    MainMenuMediumScaleUpTransition();
    virtual ~MainMenuMediumScaleUpTransition();
    virtual void init(GameWindow *);
    virtual void update(int);
    virtual void reverse();
    virtual void draw();
    virtual void skip();
protected:
    int m_startFrame,m_endFrame; //10,14
    ICoord2D m_pos,m_size; //18,20
    int m_drawState; //28
    ICoord2D m_growPos,m_growSize,m_incrementSize; //2C,34,3C
    GameWindow *m_growWin; //44
};
// Scale-up with a marker window: end frame 5 (ctor), growPos/growSize/increment at 2C..48, marker window at 4C.
class MainMenuScaleUpTransition : public Transition {
public:
    virtual ~MainMenuScaleUpTransition();
    virtual void init(GameWindow *);
    virtual void update(int);
    virtual void reverse();
    virtual void draw();
    virtual void skip();
protected:
    int m_startFrame,m_endFrame; //10,14
    ICoord2D m_pos,m_size; //18,20
    int m_drawState; //28
    ICoord2D m_growPos,m_growSize,m_incrementPos,m_incrementSize; //2C,34,3C,44
    GameWindow *m_growWin; //4C
};
typedef char MainMenuScaleUpTransitionSize[sizeof(MainMenuScaleUpTransition)==0x50?1:-1];
// Nine-step fade: ctor base 1DBAA4 writes end 10 and drawState -1; window+0x48 is the draw image.
class FadeTransition : public Transition {
public:
    virtual ~FadeTransition();
    virtual void update(int);
    virtual void draw();
    virtual void skip();
protected:
    ICoord2D m_pos,m_size; //10,18
    int m_drawState; //20
};
typedef char FadeTransitionSize[sizeof(FadeTransition)==0x24?1:-1];
// Button flash: frames 0..17, draw state at +20.
enum ButtonFlashFrame
{
    BUTTONFLASHTRANSITION_START = 0,
    BUTTONFLASHTRANSITION_FADE_IN_1 = 1,
    BUTTONFLASHTRANSITION_FADE_IN_2 = 2,
    BUTTONFLASHTRANSITION_FADE_IN_3 = 3,
    BUTTONFLASHTRANSITION_FADE_TO_BACKGROUND_1 = 4,
    BUTTONFLASHTRANSITION_FADE_TO_BACKGROUND_2 = 5,
    BUTTONFLASHTRANSITION_FADE_TO_BACKGROUND_3 = 6,
    BUTTONFLASHTRANSITION_FADE_TO_BACKGROUND_4 = 7,
    BUTTONFLASHTRANSITION_FADE_TO_GRADE_IN_1 = 11,
    BUTTONFLASHTRANSITION_FADE_TO_GRADE_IN_2,
    BUTTONFLASHTRANSITION_FADE_TO_GRADE_OUT_1,
    BUTTONFLASHTRANSITION_FADE_TO_GRADE_OUT_2,
    BUTTONFLASHTRANSITION_FADE_TO_GRADE_OUT_3,
    BUTTONFLASHTRANSITION_FADE_TO_GRADE_OUT_4,
    BUTTONFLASHTRANSITION_END,
    BUTTONFLASHTRANSITION_SHOW_BACKGROUND
};
class ButtonFlashTransition : public Transition {
public:
    virtual void init(GameWindow *);
    virtual void update(int);
    virtual void reverse();
    virtual void draw();
    virtual void skip();
protected:
    ICoord2D m_pos,m_size; //10,18
    int m_drawState; //20
};
// Flash: same 24-byte layout as FadeTransition (pos10, size18, drawState20); frames 0..8.
class FlashTransition : public Transition {
public:
    virtual ~FlashTransition();
    virtual void init(GameWindow *);
    virtual void update(int);
    virtual void draw();
    virtual void skip();
protected:
    ICoord2D m_pos,m_size; //10,18
    int m_drawState; //20
};
typedef char FlashTransitionSize[sizeof(FlashTransition)==0x24?1:-1];
typedef char ControlBarArrowTransitionSize[sizeof(ControlBarArrowTransition)==0x38?1:-1];
typedef char MainMenuSmallScaleDownTransitionSize[sizeof(MainMenuSmallScaleDownTransition)==0x40?1:-1];
typedef char MainMenuMediumScaleUpTransitionSize[sizeof(MainMenuMediumScaleUpTransition)==0x48?1:-1];

// TextType: types its window's static text one character per frame (retail 0x0035FDCA init, 0x0035FE72 update, 0x0035FF1D draw,
// 0x0035FB6A drawTypeText). Layout from those bodies: start/end frame 10/14, pos 18, size 20, state 28, the whole text 2C, the
// text typed so far 30, the display string 34.
class DisplayString;
class TextTypeTransition : public Transition {
public:
    TextTypeTransition();
    virtual ~TextTypeTransition();
    virtual void init(GameWindow *);
    virtual void update(int);
    virtual void reverse();
    virtual void draw();
    virtual void skip();
protected:
    int m_startFrame,m_endFrame; //10,14
    ICoord2D m_pos,m_size; //18,20
    int m_drawState; //28
    UnicodeString m_fullText,m_partialText; //2C,30
    DisplayString *m_dStr; //34
};
typedef char TextTypeTransitionSize[sizeof(TextTypeTransition)==0x38?1:-1];
