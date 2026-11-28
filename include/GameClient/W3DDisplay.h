#pragma once
// W3DDisplay: the Direct3D display (TheDisplay's concrete class). SIZE and its own members (from +0x144, after
// Display) are not recovered here; this header declares the overrides whose names Display.h already carries and the
// non-virtual members the ledger names, so other units can call them through `(W3DDisplay *)TheDisplay`.
// Evidence (retail): W3DDisplay's table (VA 0x007F5DA0's derived copy) overrides the Display slots in place; the
// ledger rows below carry the accessibility letter (U virtual, Q public, I protected, A private).
// The 0x0004D6B3 row (W3DDisplayDrawImageMode.cpp) is the image draw with a blend mode that the window transitions call.
#include "GameClient/Display.h"
#include "Common/IRegion2D.h"

class Image;
class DisplayString;
class Coord3D;
struct RGBColor;
class VideoBuffer;
class RTS3DScene;
class LightClass;

class W3DDisplay : public Display {
public:
	W3DDisplay();															// 0x00049DEA (the factory 0x0004C45D allocates 0x2B0 bytes)
	virtual void init();																			// 0x00046A50
	virtual void reset();																			// 0x00043418
	virtual void draw();																			// 0x0004BEF2
	virtual void setWidth(unsigned int width);														// 0x00043340
	virtual void setHeight(unsigned int height);													// 0x000433AC
	virtual bool setDisplayMode(unsigned int, unsigned int, unsigned int, bool);					// 0x00043240
	virtual void createLightPulse(const Coord3D *, const RGBColor *, float, float, unsigned int, unsigned int);	// 0x0004423C
	virtual VideoBuffer *createVideoBuffer(bool);													// 0x00044D7F
	void rva0004D664(float, float, float, float, float, int);						// 0x0004D664: draws a line (start, end, width, colour); ShellMenuScheme::draw 0x00200506 calls it
	void rva0008EEF0(float, float, float, float, float, int);						// 0x0008EEF0: outlined rectangle (x, y, w, h, line width, colour)
	void rva0004D6B3(Image *, float, float, float, float, int, int);								// 0x0004D6B3
	void takeScreenShot(const char *);																// 0x00049B3A
	void updateAverageFPS();																		// 0x000437A1
	void drawAnimStats();																			// 0x00043BF9
	void drawSkirmishAIStats();																		// 0x0004A1B8

	// The remaining slots of the table VA 0x007C3C80 (101 slots; none is __purecall): every pure virtual of Display is overridden by a W3DDisplay body in this unit.
	virtual void slot27();
	virtual bool slot28(SubTitleManager *, float, float, int, int, int, int);
	virtual void slot29();
	virtual void slot31();
	virtual void slot32();
	virtual void slot33();
	virtual void slot34();
	virtual void setClipRegion(IRegion2D *);
	virtual bool isClippingEnabled();
	virtual void enableClipping(bool);
	virtual void slot45();
	virtual void slot46();
	virtual void slot48(float);
	virtual void slot49(int, float, float, float);
	virtual void slot50(int, float, float, float);
	virtual void slot51(float, float, float, float);
	virtual void slot52(float);
	virtual void slot53();
	virtual void slot54();
	virtual void slot55();
	virtual void drawOpenRect(float, float, float, float, float, unsigned int);
	virtual void drawFillRect(float, float, float, float, unsigned int);
	virtual void slot58();
	virtual void slot59();
	virtual void slot60();
	virtual void slot61();
	virtual void drawImage(const Image *, float, float, float, float, unsigned int, int);
	virtual void slot63();
	virtual void slot64();
	virtual void slot65(Image *, float, float, float, float, int);
	virtual void slot76(int, int, int);
	virtual void slot77();
	virtual void setBorderShroudLevel(UnsignedByte alpha);
	virtual void slot79(int, int, int);
	virtual void slot80();
	virtual void slot81();
	virtual void slot82(int, int);
	virtual void slot83();
	virtual bool slot84(int, unsigned int);
	virtual void slot85();
	virtual void enableLetterBox(Int enable);
	virtual float getAverageFPS();
	virtual void slot93();
	virtual void slot94();
	virtual Render2DClass *slot95();
	virtual void slot96();
	virtual void slot99(int);

	// The members W3DDisplay adds after Display's 0x144 bytes (retail constructor 0x00049DEA, destructor 0x00049FCB, drawAnimStats
	// 0x00043BF9, drawLine 0x00044928; names are Zero Hour's where its W3DDisplay has the member):
	Byte m_initialized;															// +0x144 (constructor stores 0)
	unsigned char m_pad0x145[3];
	LightClass *m_myLight[4];		// +0x148, allocated by init and installed in the scene at 0x00046C1F
	LightClass *m_secondaryLights[4];	// +0x158, allocated by the same LightClass constructor and installed at 0x00046C2F
	Render2DClass *m_2DRender;													// +0x168 (drawLine 0x00044928 clears its flag at +0x48 and adds the line to it)
	IRegion2D m_clipRegion;														// +0x16C (drawLine passes it to the clip test 0x0025F406)
	Bool m_isClippedEnabled;													// +0x17C
	unsigned char m_pad0x17D[3];
	Real m_averageFPS;															// +0x180 (the constructor stores GlobalData +0x28 converted to float)
	int m_unk0x184;																// +0x184
	int m_unk0x188;																// +0x188
	DisplayString *m_displayStrings[16];										// +0x18C..0x1CC (the destructor releases 16 through TheDisplayStringManager)
	DisplayString *m_benchmarkDisplayString;									// +0x1CC
	DisplayString *m_animDebugStrings[25];										// +0x1D0..0x234 (drawAnimStats 0x00043BF9 walks 0x19 of them; the destructor releases 25)
	DisplayString *m_unk0x234[17];												// +0x234..0x278 (the destructor releases 17)
	unsigned int m_unk0x278;													// +0x278
	unsigned int m_unk0x27C;													// +0x27C
	unsigned char m_unk0x280[0x30];												// +0x280..0x2B0 (a 0x24-byte object constructed by 0x00045984 and a 12-byte vector)

	static RTS3DScene *m_3DScene;																	// VA 0x00DE1B34 (W3DDebrisDraw dtor 0x000B1CE2 removes its render object through it)
};
typedef char W3DDisplaySize[(sizeof(W3DDisplay) == 0x2B0) ? 1 : -1];
