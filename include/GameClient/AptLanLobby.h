// AptLanLobby.h: the LAN lobby screen ("LanLobby.apt": LanOpenPlay and LanStrategic modes).
//
// Target facts (retail game.dat, RVAs; table addresses are VAs):
//  - Constructor 0x00445EE3, destructor 0x0044455C, deleting destructor 0x00445211; SIZE 0x6C4 (factory 0x002D1F77).
//    Three tables: 0x00C3E0F8 (primary, 15 slots: _bfme_AptGameWindow's 14 plus 14 = MPOwnerValidatGameInfo), 0x00C3E0F4 (the
//    AptCallbackHost secondary, one slot) and 0x00C3E098 (the MpGameSetupOwner base at +0x27C, 23 slots, every one overridden).
//  - Layout: _bfme_AptGameWindow (0x27C), MpGameSetupOwner (+0x27C, 0xC bytes), the embedded AptMpGameSetup panel at +0x288
//    (0x3E0 bytes, constructed with the owner subobject and mode 0x400001A0), a GameSorter at +0x668 (0x1C bytes, built from
//    the callback host), LANPreferences at +0x684 (0x1C), the player name at +0x6A0, the state word at +0x6A4, the game list
//    window at +0x6A8, a window link at +0x6AC (vptr, +0x6B0, the linked name-entry window at +0x6B4), four flag bytes,
//    +0x6BC an int (2 at construction) and +0x6C0 a byte.
//  - The state word (+0x6A4): OnStartGameBttn sets 5, OnJoinGameBttn moves 1 to 7, OnLoadGameBttn moves 1 to 11, OnCreateGameBttn
//    moves 1 to 2, Cancel treats 6 apart.
//  - The owner-interface overrides (slots of the table at 0x00C3E098) read the full object through `this - 0x27C`; the map names
//    them AptLanLobby members (QAE) and they are declared non-virtual here under those names (rows not written: the map spells a
//    virtual override differently from its table slot).
#pragma once
#ifndef BFME2_APTLANLOBBY_H
#define BFME2_APTLANLOBBY_H

#include "GameClient/BfmeAptGameWindow.h"
#include "GameClient/MpGameSetupOwner.h"
#include "GameClient/AptMpGameSetup.h"
#include "Common/LANPreferences.h"
#include "GameClient/GameSorter.h"
#include "Common/UnicodeString.h"

class LANGameInfo;
// The window manager object keeps the Apt screen switch state at +0x31C (the map names no layout for it).
struct AptWindowManagerFields
{
	unsigned char m_pad[0x31C];
	int m_x31C;
};

// A link from the lobby to a window it watches (table VA 0x0083DFA8: slot 1 0x0031455E links a target, slot 2 0x00314581 unlinks it).
class AptWindowLink
{
public:
	virtual void slot0();								// 0x00444067
	virtual void Link(GameWindow *target);				// slot 1, 0x0031455E
	virtual void Unlink();								// slot 2, 0x00314581

	int m_x04;											// +0x04
	GameWindow *m_target;								// +0x08: the linked window
};

class AptLanLobby : public _bfme_AptGameWindow, public MpGameSetupOwner
{
public:
	AptLanLobby(void *);							// 0x00445EE3
	virtual ~AptLanLobby();

	virtual int rva0044522D(int, unsigned char, int);		// primary slot 1 (GameWindow::input), 0x0044522D
	virtual int rva00444826(int, unsigned int, unsigned int);	// primary slot 2 (GameWindow::system), 0x00444826
	virtual int OnUpdateData();						// primary slot 5 (GameWindow::invokeCallback), 0x00446443
	virtual void rva004442DB();						// base slot 12, 0x004442DB
	virtual void rva00446772();						// base slot 13, 0x00446772
	virtual void enableSetupUI();					// owner slot 15, 0x0044421C: the host enables the setup UI (this is the MpGameSetupOwner subobject)
	virtual void slot18(bool, int);					// owner slot 18, 0x004441C5: moves to state 12 and asks the LAN to start the game or accept
	virtual unsigned char MPOwnerValidatGameInfo(int);	// slot 14, 0x00444208
	// The MpGameSetupOwner overrides (table VA 0x00C3E098; the bodies never touch `this`).
	virtual bool MpOwnerOnReadyChecked(bool);			// slot 3, 0x00444240
	virtual bool MpOwnerSelectColor(GameSlot *, int);	// slot 4, 0x004454A1
	virtual bool MpOwnerSelectHandicap(GameSlot *, int);	// slot 5, 0x00444AC3
	virtual bool MpOwnerSelectHero(GameSlot *);		// slot 6, 0x004456B8
	virtual bool MpOwnerSelectMap(const AsciiString &);	// slot 7, 0x00444F9C
	virtual bool MpOwnerSelectPlayer(GameSlot *, SlotState, const UnicodeString &);	// slot 9, 0x00444B90
	virtual bool MpOwnerSelectPlayerTemplate(GameSlot *, int);	// slot 10, 0x004455B2
	virtual bool MpOwnerSelectStartPosition(GameSlot *, int);	// slot 11, 0x00444CA4
	virtual bool MpOwnerSelectTeam(GameSlot *, int);	// slot 12, 0x004449FD
	virtual void MpOwnerGetLocalPlayerName(UnicodeString &);	// slot 14, 0x00444D7B
	virtual void MpOwnerPrintMessage(const UnicodeString &, int);	// slot 17, 0x004448E5

	void OnStartGameBttn(const char *);
	void OnJoinGameBttn(const char *);
	void OnLoadGameBttn(const char *);
	void OnExitBttn(const char *);
	void OnOptionsBttn(const char *);
	void OnCreateGameBttn(const char *);
	void OnLoadScreen(const char *);
	void Cancel(int);
	void CancelMenu(void);
	int GetValidSelectedGameInfo();					// The selected game when TheLAN accepts it, else 0
	void rva004443E7(void);
	void OnGameCreate();								// (LANAPI::OnGameCreate calls it on the open lobby screen when the game was created; the map holds the row on the placeholder class Rva004469D1Receiver; Open-BFME-1's AptLanLobby has the same name)
	void refreshSetupBusy(bool enable);				// 0x0044446D (the map spells it rva0044446D@Rva004469D1Receiver): enable asks the setup panel to start for a guest, else marks it busy
	void InitGadgets(const char *, void *, GameWindow *);	// Binds the custom games list and the player name entry
	void enterState1();								// 0x00444357: back to the first state (the lobby's idle step)
	void rva00444083(int flags);					// Tells the movie to enable the create (1) and join (2) buttons, once each
	void rva004440F4(int flags);					// The matching disables
	void submitNameRva00444760();
	void rva00446386(bool enable);					// Puts the name entry on or takes it off the tab list
	void rva004442FD();								// Forgets the game list and unlinks the name entry
	void rva00444EC8();								// LANAPI::OnHostLeave calls it on the open lobby
	void rva00445E3E(LANGameInfo *games);			// LANAPI::OnGameList hands the lobby the game list
	void rva00444462();								// 0x00444462 (LANAPI::OnGameOptionsPacked calls it on the open lobby)

	AptMpGameSetup m_setup;							// +0x288
	GameSorter m_sorter;							// +0x668
	LANPreferences m_preferences;					// +0x684
	UnicodeString m_playerName;						// +0x6A0
	int m_state;									// +0x6A4
	GameWindow *m_gameList;							// +0x6A8
	AptWindowLink m_nameLink;						// +0x6AC: the link to the player name entry (its target at +0x6B4)
	bool m_flag6B8;									// +0x6B8
	bool m_flag6B9;									// +0x6B9
	bool m_flag6BA;									// +0x6BA
	bool m_flag6BB;									// +0x6BB
	int m_x6BC;										// +0x6BC (2 at construction)
	bool m_flag6C0;									// +0x6C0
	unsigned char m_pad6C1[3];
private:
	static AptLanLobby *s_instance;					// VA 0x00E03354: the open lobby screen (ledger name ?s_instance@AptLanLobby@@0PAV1@A)
	friend class LANAPI;
	friend void Rva00444525Init(void);				// 0x00444525: opens LanLobby.apt unless a lobby is already open
	friend void Rva00444040Enable(void);			// 0x00444040: enables the window manager while a lobby is open
};
typedef char AptLanLobbySizeCheck[sizeof(AptLanLobby)==0x6C4 ? 1 : -1];

#endif
