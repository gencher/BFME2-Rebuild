// Portions derived from Command & Conquer Generals and Command & Conquer Generals Zero Hour, (c) Electronic Arts Inc., GPL-3.0-or-later; see LICENSE and NOTICE.
// DebugIOCon.h: the debug I/O class that writes to a console window (Zero Hour's debug_io_con.cpp, internal_io.h). Target
// facts (retail game.dat): vftable 0x007C15F4; Create 0x00040E50 allocates 0x110 bytes through DebugAllocMemory. Layout from
// the constructor 0x00040C40 and Read 0x000408D0: +4 m_allocatedConsole, +5 m_input[256], +0x108 m_inputUsed, +0x10C
// m_inputRead. Slot 2 (CanRead) is the override 0x000408C0 that returns the allocated flag (the gate Zero Hour applies at the
// top of Read), slot 4 EmergencyFlush the shared `ret` 0x0069E440, slot 6 Delete the shared thunk 0x000386C0. The
// constructor differs from Zero Hour's: it installs a console control handler (0x00040830) and strips the close item from the
// system menu, and it sets the window title to the executable name plus " [debug console]".
#pragma once
#ifndef BFME2_DEBUGIOCON_H
#define BFME2_DEBUGIOCON_H

#include "debug/DebugIOInterface.h"

void DebugFreeMemory(void *ptr);

class DebugIOCon : public DebugIOInterface
{
public:
	DebugIOCon();
	virtual ~DebugIOCon();														// slot 0: 0x00040E20 (deleting), body 0x00040860

	virtual int Read(char *buf, int maxchar);									// slot 1: 0x000408D0
	virtual bool CanRead() { return m_allocatedConsole; }						// slot 2: 0x000408C0
	virtual void Write(StringType type, const char *src, const char *str);		// slot 3: 0x00040B10
	virtual void EmergencyFlush() {}											// slot 4: 0x0069E440
	virtual void Execute(Debug &dbg, const char *cmd, bool structuredCmd, unsigned int argn, const char *const *argv);	// slot 5: 0x00040B50
	virtual void Delete()														// slot 6: 0x000386C0
	{
		this->~DebugIOCon();
		DebugFreeMemory(this);
	}

	static DebugIOInterface *Create();

private:
	char m_allocatedConsole;													// +4
	char m_input[256];															// +5
	unsigned int m_inputUsed;													// +0x108
	unsigned int m_inputRead;													// +0x10C
};

#endif
