// CommandXlat.h: command translator helpers (Zero Hour GameClient/CommandXlat.h).
#pragma once
#ifndef BFME2_COMMANDXLAT_H
#define BFME2_COMMANDXLAT_H

#include "Common/MessageStream.h"
#include "Common/GameMessageTranslator.h"
#include "Common/PixelRegion.h"
#include "GameClient/DrawableList.h"
#include "GameClient/PickAndPlayInfo.h"

Bool pickAndPlayUnitVoiceResponse(const DrawableList *list, GameMessage::Type msgType, PickAndPlayInfo *info);

class CommandButton;
// Target constructor4296C2 clears these members, in this initializer order.
// The translator inherits the two real GameMessageTranslator slots.
class CommandTranslator : public GameMessageTranslator
{
public:
    // Zero Hour's three command modes; createEnterMessage 0x00429007 returns early on 2 (EVALUATE_ONLY).
    enum CommandEvaluateType { DO_COMMAND, DO_HINT, EVALUATE_ONLY };

    CommandTranslator();
    virtual ~CommandTranslator() {}
    virtual GameMessageDisposition translateGameMessage(const GameMessage *message);
    const CommandButton *findSpecialPowerCommandButton(int specialPowerType);	// 0x0042970A (role name): the first selected drawable's command set button for that special power
    int createEnterMessage(Drawable *draw, int commandType);
    int createEvacuateMessage(Drawable *draw, int commandType);
    int createDockMessage(Drawable *draw, int commandType);
    // Retail 0x00429E11 (non-virtual): the context-command evaluation GameClient forwards to.
    GameMessage::Type evaluateContextCommand(Drawable *draw, const Coord3D *pos, CommandEvaluateType cmdType);
private:
    int issueGuardCommand(const CommandButton *command, int commandType, Drawable *target, const Coord3D *pos);	// 0x00428EEB (role name)
    int createAttackMessage(Drawable *draw, Drawable *other, int commandType);	// 0x004293EE
    int issueFireWeaponCommand(const CommandButton *command, int commandType, Drawable *target, const Coord3D *pos);	// 0x00429471
    int issueMoveToLocationCommand(const Coord3D *pos, Drawable *drawableInWay, int commandType);	// 0x004292F6
    int issueAttackCommand(Drawable *target, const Coord3D *pos, int commandType, int command);	// 0x004297F0
    int issueCombatDropCommand(const CommandButton *command, int commandType, Drawable *target, const Coord3D *pos);
    int evaluateForceAttack(Drawable *draw, const Coord3D *pos, CommandEvaluateType type);
    Int m_objective; // +04, Zero Hour CommandXlat.h spelling
    Bool m_teamExists; // +08
    ICoord2D m_mouseRightDragAnchor; // +0C
    ICoord2D m_mouseRightDragLift; // +14
    UnsignedInt m_mouseRightDown; // +1C
    UnsignedInt m_mouseRightUp; // +20
    UnsignedInt m_unk0x24;
    UnsignedInt m_unk0x28;
    Bool m_unk0x2C;
};
typedef char check_CommandTranslator_size[sizeof(CommandTranslator)==0x30?1:-1];

#endif
