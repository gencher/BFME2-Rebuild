#pragma once

#include <stddef.h>
#include "Apt/AptRef.h"
#include "Common/STLTypedefs.h"
#include "Common/StlHashMap.h"
#include "Common/SubsystemInterface.h"

class AptCommandMap;
class AptCustomRender;
class AptExternHandler;
class AptTimer;
class GameWindow;
class Image;
class UnicodeString;
struct Coord2D;

// The callback HandleOverButtons runs for the button under the cursor: its second slot takes the button's
// name (or an empty string).
class AptOverButtonHandler : public AptRefTarget
{
public:
	virtual void operator()(const char *button) = 0;
};

// 0x224E10 constructs SubsystemInterface and installs the 21-slot table at
// RVA 0x7E6E80. The renderer factory at 0x41E75 allocates 0x32C bytes for its
// derived window manager, whose table changes without adding storage.
class AptPlayer : public SubsystemInterface
{
public:
	AptPlayer();
	virtual ~AptPlayer();

	virtual void init();                         // slot 1, 0x224296
	virtual void reset();                        // slot 9, 0x222471
	virtual void update();                       // slot 10, 0x2251E5
	virtual void draw();                         // slot 12, 0x22244D
	virtual void shutdown();                     // slot 14, 0x22274A
	// Renderer table RVA 0x7C57E0 supplies these abstract slots. The
	// first two results are pairs of floats; the original names are open.
	virtual const Coord2D *vslot15() = 0;
	virtual const Coord2D *vslot16() = 0;
	virtual void vslot17(const float *, const float *, float *, float *) = 0;
	virtual void vslot18(int) = 0;
	virtual void vslot19(bool) = 0;
	virtual int AddLevel(AsciiString directory, AsciiString file, bool show, int parameter);

	static int GetLevelIndex(GameWindow *window);

	void *PeekGameWindow(int level);
	bool ShowLevel(int level);
	bool isLevelUnloaded(unsigned int level);
	void PopFocus(int level);
	void setMousePosition(int *point);
	void enable();
	int invoke(void *level, const char *name, int a, const char *text, void *b, void *c, void *d, void *e);
	int invokeAndRefresh(void *level, const char *name, int a, const char *text, void *b, void *c, void *d, void *e);
	int callAndRefresh(void *level, const char *name1, const char *name2, int a, const char *text, void *b, void *c, void *d, void *e);
	void hideBackground(bool flag);
	void showBackground(int mode);
	void bfmeSetText(const AsciiString &name, const UnicodeString &text, bool visible);
	void rva002239E2(const AsciiString &name, const Image *image);
	bool UnloadLevel(int level);
	bool LoadLevel(unsigned int level);
	void updateFocus();
	int levelIndexOf(const AsciiString *name);
	void HandleOverButtons();
	bool RemoveLevel(int level);
	void RemoveOverButtonHandler(const AsciiString &name);
	void RemoveOverButtonHandler(int level, const AsciiString &name);
	void removeBackground();
	bool HideLevel(int level);
	void loadBackground();
	void unloadAllLevels();
	bool isActive();
	int getUnk0x318() const { return m_unk0x318; }	// the strategic panels ignore clicks while it is not zero

private:
	struct ExternHandlerRegistration
	{
		AptRef<AptExternHandler> m_handler;
		int m_context;
	};

	typedef _STL::hash_map<AsciiString, AptRef<AptCommandMap>, rts::hash<AsciiString>, _STL::equal_to<AsciiString> > CommandMap;
	typedef _STL::hash_map<AsciiString, ExternHandlerRegistration, rts::hash<AsciiString>, _STL::equal_to<AsciiString> > ExternHandlerMap;
	typedef _STL::hash_map<AsciiString, AptRef<AptCustomRender>, rts::hash<AsciiString>, _STL::equal_to<AsciiString> > CustomRenderMap;
	typedef _STL::hash_map<AsciiString, AptRef<AptOverButtonHandler>, rts::hash<AsciiString>, _STL::equal_to<AsciiString> > OverButtonHandlerMap;
	typedef _STL::hash_map<AsciiString, int, rts::hash<AsciiString>, _STL::equal_to<AsciiString> > MovieLevelMap;
	typedef _STL::hash_map<AsciiString, const Image *, rts::hash<AsciiString>, _STL::equal_to<AsciiString> > RenderImageMap;
	typedef _STL::hash_map<AsciiString, AptRef<AptTimer>, rts::hash<AsciiString>, _STL::equal_to<AsciiString> > TimerMap;

	CommandMap m_commands;                      // +0x0C, AddCommandMap at 0x2243FC
	ExternHandlerMap m_externalHandlers;        // +0x20, owns handler and context at 0x2244A4
	CustomRenderMap m_customRenderers;          // +0x34, AddCustomRender at 0x224666
	unsigned char m_unk0x48[0x14];              // +0x48, hash-map value type unresolved
	MovieLevelMap m_levelIndices;               // movie name to level index, filled by AddLevel
	MovieLevelMap m_reservedLevels;             // +0x70, AddLevel loads the reserved index at 0x22474D
	unsigned char m_unk0x84[0x0C];              // +0x84, tree constructed at 0x224E81
	RenderImageMap m_renderImages;              // +0x90, FindRenderImage takes the value at 0x223B23
	OverButtonHandlerMap m_globalOverButtons;   // +0xA4, AddOverButtonHandler at 0x2244F9
	TimerMap m_timers;                          // +0xB8, AddTimer at 0x2246D0

	struct AptLevel
	{
		AsciiString m_name;
		AsciiString m_directory;
		GameWindow *m_window;
		int m_index;
		OverButtonHandlerMap m_overButtons;     // +0x10, level-specific insertion at 0x2245AC
		struct Flags
		{
			unsigned char m_shown : 1;
			unsigned char m_loaded : 1;
			unsigned char m_bit2 : 1;
			unsigned char m_focused : 1;
		};
		union
		{
			unsigned char m_flags;
			Flags m_bits;
		};

		void assign(int index, const AsciiString &directory, const AsciiString &file, void *window);
		void reset();
	};
	typedef char AptLevel_size[(sizeof(AptLevel) == 0x28) ? 1 : -1];

	AptLevel m_levels[14];                    // +0xCC
	_STL::vector<int> m_focus;                // +0x2FC, level indices and sentinel values
	bool m_focusChanged;
	unsigned int m_lastTime;                 // +0x30C, timeGetTime
	bool m_enabled;
	bool m_dirty;
	bool m_refreshDeferred;
	GameWindow *m_focusWindow;                // +0x314
	int m_unk0x318;
	int m_backgroundMode;
	int m_savedBackgroundMode;
	int m_backgroundLevel;                   // +0x324, result of AddLevel
	bool m_refreshPending;
	bool m_flag329;
};

typedef char AptPlayer_size[(sizeof(AptPlayer) == 0x32C) ? 1 : -1];

extern AptPlayer *g_bfmeAptWindowManager;
int Rva002D3409Invoke(AptPlayer *player, int level, const char *name, const char *const &text);
