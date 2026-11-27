// platformsdk
// Generated from the ledger; map snapshot c591f7146f88c8a8248ebb8ae22f0c978c22aed463db64e05ef57ff0dfae99cc
// object=G00237D19 name-evidence=invented
// Extents and edges are hypotheses; byte verification alone does not establish original TU boundaries.
#include "Common/GameEngine.h"

GameEngine *CreateGameEngine();

// FUNCTION: BFME2 0x00237D19 size=36 name=?GameMain@@YAXHPAPAD@Z evidence=N:ledger tier=B hypothesis:mm1-gap-unit
void GameMain(Int argc, char **argv)
{
	TheGameEngine = CreateGameEngine();
	TheGameEngine->init(argc, argv);
	TheGameEngine->execute();
}
