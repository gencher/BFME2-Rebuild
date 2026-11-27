// Generated from the ledger; map snapshot c591f7146f88c8a8248ebb8ae22f0c978c22aed463db64e05ef57ff0dfae99cc
// object=G00313913 name-evidence=invented
// Extents and edges are hypotheses; byte verification alone does not establish original TU boundaries.
#include "GameClient/GameWindow.h"

// FUNCTION: BFME2 0x00313913 size=16 name=?vslot6@GameWindow@@UBE_NXZ evidence=N:ledger tier=B hypothesis:mm1-gap-unit
// FUNCTION: BFME2 0x00313923 size=23 name=?rva00313923@Rva00313923@@QAEXH@Z evidence=?:placeholder tier=B hypothesis:mm1-gap-unit
// FUNCTION: BFME2 0x0031393A size=16 name=?vslot8@GameWindow@@UBE_NXZ evidence=N:ledger tier=B hypothesis:mm1-gap-unit
// FUNCTION: BFME2 0x0031394A size=33 name=?orderPairs@GameWindow@@QAEXXZ evidence=S:strong (caller-this) tier=B hypothesis:mm1-gap-unit
void GameWindow::orderPairs()
{
    if (m_regionLoX > m_regionHiX)
    {
        int oldLoX = m_regionLoX;
        m_regionLoX = m_regionHiX;
        m_regionHiX = oldLoX;
    }

    if (m_regionLoY > m_regionHiY)
    {
        int oldLoY = m_regionLoY;
        m_regionLoY = m_regionHiY;
        m_regionHiY = oldLoY;
    }
}
