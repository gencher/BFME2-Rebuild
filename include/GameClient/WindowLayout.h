#pragma once
#include "Common/AsciiString.h"
class GameWindow;
class WindowLayout;
typedef void (*WindowLayoutFunc)(WindowLayout *,void *);
// SIZE0x24: native ctor538C0E stores EmptyLayout at+4, window links8/C,
// count10, hidden14 and callbacks18/1C/20. Table839608 has nine slots,
// inherited in AptWindowLayout's table8395E4. Destructor is slot1, not0.
// name: donor (ZH); addWindow538B48/removeWindow538B95/bringForward538B07
// and layout callbacks independently corroborate the identity. Old
// BfmeQuickMatchScreenBase/Rva00538B2F/Rva00538C5F denote this same class.
class WindowLayout {
public:
    WindowLayout();
    virtual void runInit(void *data);
    virtual ~WindowLayout();
    virtual void runUpdate(void *data);
    virtual void runShutdown(void *data);
    virtual void hide(bool hidden);
    virtual void bringForward();
    virtual void addWindow(GameWindow *);
    virtual void removeWindow(GameWindow *);
    virtual void destroyWindows();
    GameWindow *getFirstWindow() const { return m_windowList; }
    AsciiString getFilename() { return m_filename; }
    bool isHidden() const { return m_hidden; }
protected:
    GameWindow *findWindow(GameWindow *);
    AsciiString m_filename;
    GameWindow *m_windowList,*m_windowTail;
    int m_windowCount;
    bool m_hidden;
    WindowLayoutFunc m_init,m_update,m_shutdown;
};
typedef char WindowLayoutSizeCheck[sizeof(WindowLayout)==0x24 ? 1 : -1];
