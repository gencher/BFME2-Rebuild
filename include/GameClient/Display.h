#pragma once
#include "Common/SubsystemInterface.h"
#include "Common/AsciiString.h"
#include "Common/UnicodeString.h"
#include <stdio.h>
class View;
class VideoBuffer;
class VideoStreamInterface;
class DisplayString;
class DebugDisplayInterface;
class Image;
class GameFont;
class SubTitleManager;
class Render2DClass;
struct IRegion2D;
class Coord3D;
struct RGBColor;
enum StaticGameLODLevel;
// SIZE0x144: ctor25D489/dtor25D686, W3D ctor49DEA starts own fields144.
// Native table7F5DA0 has100 slots; SubsystemInterface occupies0..13.
// This declaration preserves
// field offsets. Known names derive from ZH and independent native callers.
// Getters16/17 read widthC/height10 (1DB0A8/1DB09D); ControlBarResizer1DB82D
// divides these by800/600. Setter map names14/15 are reversed relative to
// these offsets; retained pending map correction, not evidence for swapping.
// Movie/cinematic string lifetimes: dtor25D686 releasesB0(Unicode),D0/F0(Ascii).
// Unknown slots preserve only observed ABI; do not infer names from folds.
class Display : public SubsystemInterface {
public:
    typedef void DebugDisplayCallback(DebugDisplayInterface *, void *, FILE *);
    Display();
    virtual ~Display();
    virtual void init() {}
    virtual void reset();
    virtual void update();
    virtual void draw(); // native25C25A overrides Subsystem slot12
    virtual void setWidth(unsigned int width);
    virtual void setHeight(unsigned int height);
    virtual unsigned int getWidth() { return m_width; }
    virtual unsigned int getHeight() { return m_height; }
    virtual void setBitDepth(unsigned int depth) { m_bitDepth = depth; }
    virtual unsigned int getBitDepth() { return m_bitDepth; }
    virtual void setWindowed(bool windowed) { m_windowed = windowed; }
    virtual bool getWindowed() { return m_windowed; }
    virtual bool setDisplayMode(unsigned int, unsigned int, unsigned int, bool);
    virtual int getDisplayModeCount();
    virtual void getDisplayModeDescription(int modeIndex, int *xres, int *yres, int *bitDepth);
    virtual void setGamma(float gamma, float bright, float contrast, bool calibrate);
    virtual bool testMinSpecRequirements(bool *, bool *, bool *, StaticGameLODLevel * = 0, float * = 0);
    virtual void slot27() = 0;
    virtual bool slot28(SubTitleManager *, float, float, int, int, int, int) = 0;
    virtual void slot29() = 0;
    virtual void slot30(const void *);
    virtual void slot31() = 0;
    virtual void slot32() = 0;
    virtual void slot33() = 0;
    virtual void slot34() = 0;
    virtual void rva0025C134(View *);
    virtual View *getFirstView() { return m_viewList; }
    virtual View *getNextView(View *);
    virtual void rva0025C14D();
    virtual void rva0025C18B(void *, void *);
    virtual void rva0025C238();
    virtual VideoBuffer *createVideoBuffer(bool) = 0;
    virtual void setClipRegion(IRegion2D *) = 0;
    virtual bool isClippingEnabled() = 0;
    virtual void enableClipping(bool) = 0;
    virtual void slot45() = 0;
    virtual void slot46() = 0;
    virtual void createLightPulse(const Coord3D *, const RGBColor *, float, float, unsigned int, unsigned int) = 0;
    virtual void slot48(float) = 0;
    virtual void slot49(int, float, float, float) = 0;
    virtual void slot50(int, float, float, float) = 0;
    virtual void slot51(float, float, float, float) = 0;
    virtual void slot52(float) = 0;
    virtual void slot53() = 0;
    virtual void slot54() = 0;
    virtual void slot55() = 0;
    void rva0004263F(float, float, float, float, int); // 0x0004263F: filled rectangle (x, y, w, h, colour)
    virtual void drawOpenRect(float, float, float, float, float, unsigned int) = 0;
    virtual void drawFillRect(float, float, float, float, unsigned int) = 0;
    virtual void slot58() = 0;
    virtual void slot59() = 0;
    virtual void slot60() = 0;
    virtual void slot61() = 0;
    virtual void drawImage(const Image *, float, float, float, float, unsigned int = 0xFFFFFFFF, int = 2) = 0;
    virtual void slot63() = 0;
    virtual void slot64() = 0;
    virtual void slot65(Image *, float, float, float, float, int) = 0;
    virtual bool rva0025C97E(AsciiString, int, int, int);
    virtual bool slot67(AsciiString, bool, int);
    virtual void stopMovie();
    virtual bool isMoviePlaying() { return field38 != 0; }
    virtual bool slot70();
    virtual bool slot71();
    virtual bool rva0025DA8D(AsciiString, int);
    virtual unsigned int rva0025CF2A(int);
    virtual void setDebugDisplayCallback(DebugDisplayCallback *, void * = 0);
    virtual DebugDisplayCallback *getDebugDisplayCallback() { return m_debugDisplayCallback; }
    virtual void slot76(int, int, int) = 0;
    virtual void slot77() = 0;
    virtual void setBorderShroudLevel(UnsignedByte alpha) = 0;	// slot 78 (+0x138): ScriptActions::doSetBorderShroud 0x003BBB51 passes the ShroudAlpha or ClearAlpha byte of the global data; Zero Hour's name
    virtual void slot79(int, int, int) = 0;
    virtual void slot80() = 0;
    virtual void slot81() = 0;
    virtual void slot82(int, int) = 0;
    virtual void slot83() = 0;
    virtual bool slot84(int, unsigned int) = 0;
    virtual void slot85() = 0;
    virtual void enableLetterBox(Int enable) = 0;	// slot 86 (+0x158): ScriptActions::doLetterBoxMode 0x003BBE04 pushes 1 or 0; Zero Hour's name (its argument is a Bool there; retail pushes a 32-bit immediate)
    virtual bool slot87() { return false; }
    virtual bool slot88() { return fieldD8; }
    virtual void setCinematicText(AsciiString);
    virtual void setCinematicFont(GameFont *);
    virtual void setCinematicTextFrames(int);
    virtual float getAverageFPS() = 0;
    virtual void slot93() = 0;
    virtual void slot94() = 0;
    virtual Render2DClass *slot95() = 0;
    virtual void slot96() = 0;
    virtual void slot97();
    virtual void slot98();
    virtual void slot99(int) = 0;

    void bfmeTickVM0();
    void StartRenderThread(int);
    void Rva006F2890RegisterVideoBuffer(VideoBuffer *);
    void rva002B2466(float, float, float, float);
    void rva0025C46E();
    void rva0025D9CB(bool);
    void rva0025D10F();
    void rva0025D19E();
    void rva0025D2F6();	// 0x0025D2F6: clears the three image slots, the text offsets and the cinematic text (the map spells it with class W3DDisplay; Display::reset 0x0025D9E3 calls it on its own this)
    void rva0025D358(UnicodeString, float, float, int, int, int);
    void rva00049F94(AsciiString);
    void SetMovieImage(int, int, float, float, float, float);
    void SetStaticImage(int, int, float, float, float, float);
    int rva0025C4A4(int);
    void rva0025C776(int, float, float, float, float, int, int);
    void DrawStaticImage(int, int);
    bool rva0025CC7A(bool);
    DisplayString *rva0025D3C8();
    void releaseTextString(DisplayString *string);   // 0x0025C7F9 (role name: frees a display string through the manager and zeroes fieldC8/fieldCC)

protected:
    unsigned int m_width;
    unsigned int m_height;
    unsigned int m_bitDepth;
    bool m_windowed;
    unsigned char pad19[3];
    View * m_viewList;
    float field20;
    float field24;
    float field28;
    unsigned int field2C;
    DebugDisplayCallback * m_debugDisplayCallback;
    void * m_debugDisplayUserData;
    VideoStreamInterface *field38;	// the playing movie (VideoStreamInterface slot 7, close, is called by 0x0025D19E)
    unsigned int field3C;
    void *volatile field40; // handle waited on and released through ReleaseMutex by 0x0025C46E (volatile: retail re-reads both)
    void *volatile field44; // handle released by 0x0025C46E
    bool field48;
    unsigned char pad49[3];
    unsigned int field4C;
    unsigned int field50;
    unsigned int field54;
    unsigned int field58;
    bool field5C;
    bool field5D;
    unsigned char pad5E[2];
    unsigned int field60;
    bool field64;
    unsigned char pad65[3];
    // Three full-screen image slots (SetMovieImage 0x0025C6E2 and SetStaticImage 0x0025C72C fill one,
    // DrawStaticImage 0x0025D33B draws one through the slot's own method 0x0025C829).
    struct ImageSlot {
        int image;           // +0x00
        float left;          // +0x04
        float top;           // +0x08
        float right;         // +0x0C
        float bottom;        // +0x10
        int kind;            // +0x14 1 = movie, 2 = static
        void draw(Display *display, int mode);   // 0x0025C829 (the map still spells it rva0025C829@Rva0025C6E2Elem with a W3DDisplay parameter)
    };
    ImageSlot m_imageSlots[3];       // +0x68
    UnicodeString fieldB0;
    unsigned int fieldB4;
    unsigned int fieldB8;
    unsigned int fieldBC;
    float fieldC0;
    float fieldC4;
    float fieldC8;
    float fieldCC;
    AsciiString fieldD0;
    float fieldD4;
    bool fieldD8;
    unsigned char padD9[3];
    unsigned int fieldDC;
    unsigned int fieldE0;
    unsigned int fieldE4;
    unsigned int fieldE8;
    unsigned int fieldEC;
    AsciiString fieldF0;
    GameFont *fieldF4;
    unsigned int fieldF8;
    float fieldFC;
    float field100;
    float field104;
    float field108;
    bool field10C;
    unsigned char pad10D[3];
    unsigned int field110;
    bool field114;
    bool field115;
    unsigned char pad116[2];
    unsigned int field118;
    float field11C;
    float field120;
    unsigned int field124;
    float field128;
    float field12C;
    float field130;
    float field134;
    unsigned int field138;
    unsigned int field13C;
    bool m_bitDepth0;
    bool m_bitDepth1;
    unsigned char pad142[2];
};

extern Display *TheDisplay;

typedef char DisplaySizeCheck[sizeof(Display)==0x144?1:-1];
