// GameEngine installs its 40-slot table at 0x007E7188 after constructing
// SubsystemInterface at 0x0022DA7A. Win32GameEngine's first member at +0x60
// (SetErrorMode result, 0x00041D56) bounds the base object.
#pragma once

#include <windows.h>
#include "Common/SubsystemInterface.h"

class AptPlayer;
class AudioManager;
class FXParticleSystemManager;
class FunctionLexicon;
class GameClient;
class GameLogic;
class MessageStream;
class ModuleFactory;
class Radar;
class Rva0060061A;
class ThingFactory;

class GameEngine : public SubsystemInterface
{
public:
	GameEngine();											// 0x0022DA66
	virtual ~GameEngine();									// 0x00225B9B
	virtual void init() {}									// Slot 1: bare ret at 0x000B3FD0.
	virtual void reset();									// 0x00229708
	virtual void update();									// 0x00225DA9

	virtual void init(Int argc, char **argv);				// 0x0022E2E4
	virtual void execute();									// 0x0022D290, updates until m_quitting.
	virtual Bool notifySubsystems();						// 0x00225AD5, gated call to 0x001B505F.
	virtual Bool refreshSubsystems();						// 0x00225AFF, refreshes then clears reload states.
	virtual void setFramesPerSecondLimit(Int fps) { m_frameRateLimit = fps; }	// 0x005F69C4
	virtual Int getFramesPerSecondLimit() { return m_frameRateLimit; }	// 0x001DB0A8
	virtual void setQuitting(Bool quitting) { m_quitting = quitting; }	// 0x000D1A7E
	virtual Bool getQuitting() { return m_quitting; }		// 0x001DBA6D
	virtual bool isMultiplayerSession();						// 0x00225F76
	virtual void serviceWindowsOS() {}						// Slot 23: bare ret, Win32 override pumps messages.
	virtual Bool isActive() { return m_isActive; }			// 0x00041D5E
	virtual void setIsActive(Bool active) { m_isActive = active; }	// 0x0023C497
	virtual void shutdown();								// 0x0023054C, releases global subsystems.

protected:
	// The initialisation routine passes all these factory results to typed
	// subsystem initializers. Slot 27's result is the settings object built
	// by 0x00600611, whose class spelling remains unresolved.
	virtual Rva0060061A *vfunc_0x6C();						// 0x00225F44, consumed at 0x0022E3AB.
	virtual GameLogic *createGameLogic() = 0;				// 0x0022F67C
	virtual GameClient *createGameClient() = 0;				// 0x0022F342
	virtual MessageStream *createMessageStream();			// 0x00225F12
	virtual ModuleFactory *createModuleFactory() = 0;		// 0x0022E91E
	virtual ThingFactory *createThingFactory() = 0;			// 0x0022EFE1
	virtual FunctionLexicon *createFunctionLexicon() = 0;	// 0x0022E8F2
	virtual Radar *createRadar() = 0;						// 0x0022F6CD
	virtual FXParticleSystemManager *createFXParticleSystemManager() = 0;	// 0x0022EAF6
	virtual AudioManager *createAudioManager() = 0;			// 0x0022E6BB
	virtual AptPlayer *createAptPlayer() = 0;				// 0x0022F1B5

public:
	virtual void _bfme_updateNetworkAndLogic(Int frame);	// 0x0022634F
	virtual void _bfme_updateClientSubsystems();			// 0x00225C12

	enum { MAX_HEADLESS_CLIENTS = 7 };

private:
	Int m_frameRateLimit;									// +0x0C, used for frame pacing in execute.
	Bool m_quitting;										// +0x10, tested at 0x0022D45E.
	Bool m_isActive;											// +0x11, Win32 focus flag (same accessors as Zero Hour).
	Int m_headlessClientCount;								// +0x14
	HANDLE m_headlessClients[MAX_HEADLESS_CLIENTS];			// +0x18, from CreateProcess.

public:
	Int m_clientFrameInLogicFrame;							// +0x34, numerator in updateClientFrameRatio.
	Int m_maxFPS;											// +0x38, client frames per logic frame.
	Real m_clientFrameRatio;									// +0x3C, used by Drawable's interpolation.

private:
	UnsignedByte m_unk0x40[0x20];

	void updateClientFrameCounter();						// 0x00225D68
	void updateClientFrameRatio();							// 0x00225D78
	void stopHeadlessClients();								// 0x002260F7, called by AptLanLobby.
	friend class AptLanLobby;
};

typedef char GameEngineSize[(sizeof(GameEngine) == 0x60) ? 1 : -1];
typedef char GameEngineFrameRatioOffset[(offsetof(GameEngine, m_clientFrameRatio) == 0x3C) ? 1 : -1];

extern GameEngine *TheGameEngine;							// 0x00DFE710

