// GlobalData.h: the GameData INI singleton (TheWritableGlobalData, VA 0x00DFE758). SIZE 0x1254.
// Target facts:
//  - Base SubsystemInterface +0x00: ctor 0x0023631C calls SubsystemInterface()
//    and stores its own table 0x00BED1C4.
//  - Size 0x1254: GameEngine::init 0x0022E3A0 allocates 0x1254 before the ctor.
//  - Members +0xC..+0x124C: the game's own GameData parse table (VA 0x00BE8520,
//    457 entries). Each member sits at
//    its entry's offset with the type its parse function writes; the member name
//    is the INI token in lower camel case. Where several tokens share an offset
//    the first is the name and the others are listed. Byte gaps are offsets the
//    table never writes.
//  - The 51 RGBColor entries are Common/RGBColor.h members (explicit pad bytes
//    before some of them are the alignment the earlier byte-gap form needed).
//  - +0x1240 AsciiString and +0x1244 UnicodeString: the getters 0x002360DE and
//    0x002360FC copy them out.
//  - +0x1250 m_next: reset 0x00235736 walks the override chain through it back
//    to s_theOriginal (VA 0x00DFE75C).
#pragma once
#ifndef BFME2_GLOBALDATA_H
#define BFME2_GLOBALDATA_H

#include <stddef.h>
#include "Common/GameType.h"
#include "Common/AsciiString.h"
#include "Common/Version.h"
#include "Common/UnicodeString.h"
#include "Common/Coord2D.h"
#include "Common/Coord3D.h"
#include "Common/RGBColor.h"
#include "Common/MultiPlayMults.h"
#include "Common/PixelRegion.h"
#include "Common/RandomValue.h"
#include "Common/Vector.h"
#include "Common/SubsystemInterface.h"
#include "GameLogic/ObjectFilter.h"

class WeaponBonusSet;

class GlobalData : public SubsystemInterface
{
public:
	// One light of one time of day: the parse table gives each row three
	// consecutive entries (RGBColor ambient +0, RGBColor diffuse +0xC, Coord3D
	// lightPos +0x18), 0x24 bytes per light.
	struct TerrainLighting
	{
		RGBColor ambient;
		RGBColor diffuse;
		Coord3D lightPos;
	};

	// The BasicTutorial* and AdvancedTutorial* tokens of one tutorial (0x24 bytes);
	// the parse table writes each field at its offset from +0x1114 and +0x1138.
	struct TutorialInfo
	{
		AsciiString map;									// +0x00 ...TutorialMap
		AsciiString loadMovie;								// +0x04 ...TutorialLoadMovie
		AsciiString loadScreenStillImage;					// +0x08 ...TutorialLoadScreenStillImage
		AsciiString loadScreenVoiceTrack;					// +0x0C ...TutorialLoadScreenVoiceTrack
		AsciiString loadScreenMusicTrack;					// +0x10 ...TutorialLoadScreenMusicTrack
		_STL::vector<AsciiString> objective;				// +0x14 ...TutorialObjective
		UnsignedInt millisecondsAfterStartToStartFadeUp;	// +0x20 ...TutorialMillisecondsAfterStartToStartFadeUp; duration, frames
	};

	GlobalData();				// 0x0023631C
	virtual ~GlobalData();			// 0x002376CC

	virtual void init() {}
	virtual void update() {}
	virtual void reset();			// 0x00235736, primary table slot9
	virtual Bool attemptReload(Bool *needsRestart);
	virtual const FieldParse *getFieldParse() const; // 0x002376C6, slot6

	GlobalData *newOverride();		// 0x00237A6B
	static void parseGameDataDefinition(INI *ini);
	AsciiString rva002360DE() const;	// 0x002360DE, copies m_unk0x1240
	UnicodeString rva002360FC() const;	// 0x002360FC, copies m_unk0x1244
	AsciiString getPicturePath() const;	// 0x0023611A
	void applyOptionPreferences();		// 0x0023576A
	Bool setTimeOfDay(TimeOfDay tod);	// 0x002352BC

	static GlobalData *s_theOriginal;	// VA 0x00DFE75C
	static const FieldParse s_GlobalDataFieldParseTable[];	// the GameData parse table (457 entries)

	AsciiString m_mapName;                                 // +0xC MapName
	AsciiString m_moveHintName;                            // +0x10 MoveHintName
	AsciiString m_genericDamageFieldName;                  // +0x14 GenericDamageFieldName
	AsciiString m_genericDamageWarningName;                // +0x18 GenericDamageWarningName
	Bool m_showProps;                                      // +0x1C ShowProps
	Bool m_pushAsideShrubs;                                // +0x1D PushAsideShrubs
	unsigned char m_unk0x1E[0x8];
	Bool m_useFPSLimit;                                    // +0x26 UseFPSLimit
	Bool m_dumpAssetUsage;                                 // +0x27 DumpAssetUsage
	Int m_framesPerSecondLimit;                            // +0x28 FramesPerSecondLimit
	Bool m_windowed;                                       // +0x2C Windowed
	Bool m_skipMapUnroll;                                  // +0x2D SkipMapUnroll
	Int m_xResolution;                                     // +0x30 XResolution
	Int m_yResolution;                                     // +0x34 YResolution
	Int m_maxShellScreens;                                 // +0x38 MaxShellScreens
	Bool m_useCloudMap;                                    // +0x3C UseCloudMap
	Bool m_showWater;                                      // +0x3D ShowWater
	Bool m_showRoads;                                      // +0x3E ShowRoads
	Bool m_showTrees;                                      // +0x3F ShowTrees
	Bool m_allowTreeFading;                                // +0x40 AllowTreeFading
	Bool m_useReverseMouseScroll;                          // +0x41 UseReverseMouseScroll
	Int m_use3WayTerrainBlends;                            // +0x44 Use3WayTerrainBlends
	Bool m_useLightMap;                                    // +0x48 UseLightMap
	unsigned char m_unk0x49[0x2];
	Bool m_adjustCliffTextures;                            // +0x4B AdjustCliffTextures
	Bool m_stretchTerrain;                                 // +0x4C StretchTerrain
	Bool m_useHalfHeightMap;                               // +0x4D UseHalfHeightMap
	Bool m_drawEntireTerrain;                              // +0x4E DrawEntireTerrain
	Int m_terrainLOD;                                      // +0x50 TerrainLOD; index into a name table
	unsigned char m_unk0x54[0x4];
	Int m_terrainLODTargetTimeMS;                          // +0x58 TerrainLODTargetTimeMS
	Bool m_alternateMouseSetup;                            // +0x5C default for the AlternateMouseSetup preference (OptionPreferences 0x002E4D4E)
	Bool m_rightMouseAlwaysScrolls;                        // +0x5D RightMouseAlwaysScrolls
	unsigned char m_unk0x5E[0x1];
	Bool m_useCloudPlane;                                  // +0x5F UseCloudPlane
	Bool m_useShadowVolumes;                               // +0x60 UseShadowVolumes
	Bool m_useShadowDecals;                                // +0x61 UseShadowDecals
	Bool m_useShadowMapping;                               // +0x62 UseShadowMapping
	unsigned char m_unk0x63[0x5];
	Bool m_useBehindBuildingMarker;                        // +0x68 UseBehindBuildingMarker
	Real m_waterPositionX;                                 // +0x6C WaterPositionX
	Real m_waterPositionY;                                 // +0x70 WaterPositionY
	Real m_waterPositionZ;                                 // +0x74 WaterPositionZ
	Real m_waterExtentX;                                   // +0x78 WaterExtentX
	Real m_waterExtentY;                                   // +0x7C WaterExtentY
	Int m_waterType;                                       // +0x80 WaterType
	Bool m_showSoftWaterEdge;                              // +0x84 ShowSoftWaterEdge
	unsigned char m_unk0x85[0x1];
	Bool m_liveCampaignMode;                               // +0x86 LiveCampaignMode
	Bool m_hideLivingWorldRegions;                         // +0x87 HideLivingWorldRegions
	Bool m_livingWorldTurbo;                               // +0x88 LivingWorldTurbo
	unsigned char m_unk0x89[0x3];
	// +0x8C: an AsciiString (the constructor clears it with the strings, the destructor releases it),
	// although the parse table reads its LivingWorldCampaignOverrride token with parseBool.
	AsciiString m_livingWorldCampaignOverrride;            // +0x8C LivingWorldCampaignOverrride
	Int m_featherWater;                                    // +0x90 FeatherWater
	Real m_downwindAngle;                                  // +0x94 DownwindAngle
	Real m_drawSkyBox;                                     // +0x98 DrawSkyBox; a Real (the constructor and the reload path treat it as one) though the table parses it with parseBool
	Real m_defaultCameraMinHeight;                         // +0x9C DefaultCameraMinHeight
	Real m_defaultCameraMaxHeight;                         // +0xA0 DefaultCameraMaxHeight
	Real m_defaultCameraPitchAngle;                        // +0xA4 DefaultCameraPitchAngle
	Real m_defaultCameraYawAngle;                          // +0xA8 DefaultCameraYawAngle
	Real m_defaultCameraScrollSpeedScalar;                 // +0xAC DefaultCameraScrollSpeedScalar
	Real m_terrainHeightAtEdgeOfMap;                       // +0xB0 TerrainHeightAtEdgeOfMap
	Real m_unitDamagedThreshold;                           // +0xB4 UnitDamagedThreshold
	Real m_unitReallyDamagedThreshold;                     // +0xB8 UnitReallyDamagedThreshold
	Real m_groundStiffness;                                // +0xBC GroundStiffness
	Real m_structureStiffness;                             // +0xC0 StructureStiffness
	Real m_gravity;                                        // +0xC4 Gravity; acceleration
	Real m_stealthFriendlyOpacity;                         // +0xC8 StealthFriendlyOpacity; percent
	UnsignedInt m_defaultOcclusionDelay;                   // +0xCC DefaultOcclusionDelay; duration, frames
	unsigned char m_unk0xD0[0x4];
	Real m_partitionCellSize;                              // +0xD4 PartitionCellSize
	Real m_terrainResourceCellSize;                        // +0xD8 TerrainResourceCellSize
	Coord3D m_ammoPipWorldOffset;                          // +0xDC AmmoPipWorldOffset
	Coord3D m_containerPipWorldOffset;                     // +0xE8 ContainerPipWorldOffset
	Coord2D m_ammoPipScreenOffset;                         // +0xF4 AmmoPipScreenOffset
	Coord2D m_containerPipScreenOffset;                    // +0xFC ContainerPipScreenOffset
	Real m_ammoPipScaleFactor;                             // +0x104 AmmoPipScaleFactor
	Real m_containerPipScaleFactor;                        // +0x108 ContainerPipScaleFactor
	Int m_maxTerrainTracks;                                // +0x10C MaxTerrainTracks
	unsigned char m_unk0x110[0xC];
	AsciiString m_levelGainAnimationName;                  // +0x11C LevelGainAnimationName
	Real m_levelGainAnimationTime;                         // +0x120 LevelGainAnimationTime
	Real m_levelGainAnimationZRise;                        // +0x124 LevelGainAnimationZRise
	AsciiString m_getHealedAnimationName;                  // +0x128 GetHealedAnimationName
	Real m_getHealedAnimationTime;                         // +0x12C GetHealedAnimationTime
	Real m_getHealedAnimationZRise;                        // +0x130 GetHealedAnimationZRise
	Int m_timeOfDay;                                       // +0x134 TimeOfDay; index into a name table
	Int m_weather;                                         // +0x138 Weather; index into a name table
	Bool m_makeTrackMarks;                                 // +0x13C MakeTrackMarks
	Bool m_hideGarrisonFlags;                              // +0x13D HideGarrisonFlags
	Bool m_forceModelsToFollowTimeOfDay;                   // +0x13E ForceModelsToFollowTimeOfDay
	Bool m_forceModelsToFollowWeather;                     // +0x13F ForceModelsToFollowWeather
	// +0x140 and +0x3C8: two [TIME_OF_DAY_COUNT][MAX_GLOBAL_LIGHTS] tables of
	// TerrainLighting. The parse table writes the morning..night rows of each
	// (TerrainLighting<Time>Ambient/Diffuse/LightPos[2|3], TerrainObjectsLighting...)
	// at 0x6C per time of day and 0x24 per light; row 0 (NONE) is never parsed.
	TerrainLighting m_terrainLighting[TIME_OF_DAY_COUNT][MAX_GLOBAL_LIGHTS];		// +0x140
	TerrainLighting m_terrainObjectsLighting[TIME_OF_DAY_COUNT][MAX_GLOBAL_LIGHTS];	// +0x3C8
	// +0x650: a third table the parse table never writes; the blend interpolates it
	// row by row with the same strides, and the destructor runs the element
	// destructors over it like the two above. Its name is unknown.
	TerrainLighting m_unk0x650[TIME_OF_DAY_COUNT][MAX_GLOBAL_LIGHTS];
	// The lighting in use: setTimeOfDay 0x002352BC copies the chosen row of
	// m_terrainLighting into these three arrays, light by light.
	RGBColor m_terrainAmbient[MAX_GLOBAL_LIGHTS];		// +0x8D8
	RGBColor m_terrainDiffuse[MAX_GLOBAL_LIGHTS];		// +0x8FC
	Coord3D m_terrainLightPos[MAX_GLOBAL_LIGHTS];		// +0x920
	unsigned char m_unk0x944[0x10];
	Real m_humanSoloPlayerHealthBonusEasy;                 // +0x954 HumanSoloPlayerHealthBonus_Easy; percent
	Real m_humanSoloPlayerHealthBonusNormal;               // +0x958 HumanSoloPlayerHealthBonus_Normal; percent
	Real m_humanSoloPlayerHealthBonusHard;                 // +0x95C HumanSoloPlayerHealthBonus_Hard; percent
	unsigned char m_unk0x960[0x4];
	Real m_aiSoloPlayerHealthBonusEasy;                    // +0x964 AISoloPlayerHealthBonus_Easy; percent
	Real m_aiSoloPlayerHealthBonusNormal;                  // +0x968 AISoloPlayerHealthBonus_Normal; percent
	Real m_aiSoloPlayerHealthBonusHard;                    // +0x96C AISoloPlayerHealthBonus_Hard; percent
	unsigned char m_unk0x970[0x4];
	Int m_maxTranslucentObjects;                           // +0x974 MaxTranslucentObjects
	unsigned char m_unk0x978[0xC];
	Real m_occludedColorLuminanceScale;                    // +0x984 OccludedColorLuminanceScale
	Int m_numberGlobalLights;                              // +0x988 NumberGlobalLights
	Int m_maxRoadSegments;                                 // +0x98C MaxRoadSegments
	Int m_maxRoadVertex;                                   // +0x990 MaxRoadVertex
	Int m_maxRoadIndex;                                    // +0x994 MaxRoadIndex
	Int m_maxRoadTypes;                                    // +0x998 MaxRoadTypes
	Bool m_audioOn;                                        // +0x99C AudioOn
	Bool m_musicOn;                                        // +0x99D MusicOn
	Bool m_soundsOn;                                       // +0x99E SoundsOn
	Bool m_sounds3DOn;                                     // +0x99F Sounds3DOn
	Bool m_speechOn;                                       // +0x9A0 SpeechOn
	Bool m_ambientStreamsOn;                               // +0x9A1 AmbientStreamsOn
	unsigned char m_unk0x9A2[0x1];
	Bool m_videoOn;                                        // +0x9A3 VideoOn
	Bool m_disableCameraMovements;                         // +0x9A4 DisableCameraMovements
	Bool m_showSelectedUnitMarker;                         // +0x9A5 ShowSelectedUnitMarker
	Bool m_useSimpleHordeDecals;                           // +0x9A6 UseSimpleHordeDecals
	Bool m_useSimpleMergeDecals;                           // +0x9A7 UseSimpleMergeDecals
	Real m_opacityOfSimpleMergeDecals;                     // +0x9A8 OpacityOfSimpleMergeDecals; percent
	unsigned char m_unk0x9AC;
	Bool m_unk0x9AD;	// +0x9AD: stored at 0x0023673C; GameLogic::setMissionName 0x0023DC8E drops the name while it is set
	unsigned char m_unk0x9AE[0x2];
	Bool m_showClientPhysics;                              // +0x9B0 ShowClientPhysics
	Bool m_showTerrainNormals;                             // +0x9B1 ShowTerrainNormals
	unsigned char m_unk0x9B2[0x6];
	Int m_debugAI;                                         // +0x9B8 DebugAI (Zero Hour's AIDebugOptions; parsed with parseBool; AIPlayer::isAGoodIdeaToBuildTeam 0x004F1566 tests the dword)
	Bool m_debugAIObstacles;                               // +0x9BC DebugAIObstacles
	Bool m_showObjectHealth;                               // +0x9BD ShowObjectHealth
	unsigned char m_unk0x9BE[0x1];
	Bool m_showTooltips;                                   // +0x9BF ShowTooltips
	Bool m_unk0x9C0;                                       // +0x9C0 set by the command-line parser 0x003B9AB9
	unsigned char m_unk0x9C1[5];
	bool m_winCursors;						// +0x9C6 Mouse::init 0x001EE4DC selects the Windows-drawn cursor mode when it is set (name: Zero Hour's)
	unsigned char m_unk0x9C7[0x9];
	Bool m_unk0x9D0;						// +0x9D0 set by a command-line option handler 0x003B986A
	Bool m_unk0x9D1;						// +0x9D1 ctor 0x002367C9 clears it; option handler 0x003B9875 sets it; AptCreateAHero::CreateAHeroDemo 0x00513A6D tests it
	unsigned char m_unk0x9D2[2];
	Int m_unk0x9D4;		// +0x9D4: signed; the constructor zeroes it, the player template parser compares it and GameSlot::setPlayerTemplate reads its low two bits
	unsigned char m_unk0x9D8[4];
	AsciiString m_defaultUnitHealingBuffFxList;            // +0x9DC DefaultUnitHealingBuffFxList
	AsciiString m_defaultStructureRepairBuffFxList;        // +0x9E0 DefaultStructureRepairBuffFxList
	unsigned char m_unk0x9E4[0x4];
	Int m_fixedSeed;                                       // +0x9E8 FixedSeed
	Real m_particleScale;                                  // +0x9EC ParticleScale
	AsciiString m_autoFireParticleSmallPrefix;             // +0x9F0 AutoFireParticleSmallPrefix
	AsciiString m_autoFireParticleSmallSystem;             // +0x9F4 AutoFireParticleSmallSystem
	Int m_autoFireParticleSmallMax;                        // +0x9F8 AutoFireParticleSmallMax
	AsciiString m_autoFireParticleMediumPrefix;            // +0x9FC AutoFireParticleMediumPrefix
	AsciiString m_autoFireParticleMediumSystem;            // +0xA00 AutoFireParticleMediumSystem
	Int m_autoFireParticleMediumMax;                       // +0xA04 AutoFireParticleMediumMax
	AsciiString m_autoFireParticleLargePrefix;             // +0xA08 AutoFireParticleLargePrefix
	AsciiString m_autoFireParticleLargeSystem;             // +0xA0C AutoFireParticleLargeSystem
	Int m_autoFireParticleLargeMax;                        // +0xA10 AutoFireParticleLargeMax
	AsciiString m_autoSmokeParticleSmallPrefix;            // +0xA14 AutoSmokeParticleSmallPrefix
	AsciiString m_autoSmokeParticleSmallSystem;            // +0xA18 AutoSmokeParticleSmallSystem
	Int m_autoSmokeParticleSmallMax;                       // +0xA1C AutoSmokeParticleSmallMax
	AsciiString m_autoSmokeParticleMediumPrefix;           // +0xA20 AutoSmokeParticleMediumPrefix
	AsciiString m_autoSmokeParticleMediumSystem;           // +0xA24 AutoSmokeParticleMediumSystem
	Int m_autoSmokeParticleMediumMax;                      // +0xA28 AutoSmokeParticleMediumMax
	AsciiString m_autoSmokeParticleLargePrefix;            // +0xA2C AutoSmokeParticleLargePrefix
	AsciiString m_autoSmokeParticleLargeSystem;            // +0xA30 AutoSmokeParticleLargeSystem
	Int m_autoSmokeParticleLargeMax;                       // +0xA34 AutoSmokeParticleLargeMax
	AsciiString m_autoAflameParticlePrefix;                // +0xA38 AutoAflameParticlePrefix
	AsciiString m_autoAflameParticleSystem;                // +0xA3C AutoAflameParticleSystem
	Int m_autoAflameParticleMax;                           // +0xA40 AutoAflameParticleMax
	unsigned char m_unk0xA44[4];
	UnsignedInt m_defaultIP;                               // +0xA48 set from OptionPreferences::getLANIPAddress by applyOptionPreferences 0x0023576A (Zero Hour's name)
	Int m_firewallBehavior;                                // +0xA4C FirewallBehavior
	Bool m_firewallSendDelay;                              // +0xA50 default for the SendDelay preference (OptionPreferences 0x002E47B0)
	unsigned char m_unk0xA51[0x3];
	Int m_firewallPortOverride;                            // +0xA54 FirewallPortOverride
	Short m_firewallPortAllocationDelta;                   // +0xA58 FirewallPortAllocationDelta (a 16-bit store in applyOptionPreferences 0x0023576A; Zero Hour declares it Short)
	unsigned char m_unk0xA5A[2];
	Int m_valuePerSupplyBox;                               // +0xA5C ValuePerSupplyBox
	Int m_supplyBoxesPerTree;                              // +0xA60 SupplyBoxesPerTree
	Real m_buildSpeed;                                     // +0xA64 BuildSpeed
	Real m_minDistFromEdgeOfMapForBuild;                   // +0xA68 MinDistFromEdgeOfMapForBuild
	Real m_supplyBuildBorder;                              // +0xA6C SupplyBuildBorder
	Real m_allowedHeightVariationForBuilding;              // +0xA70 AllowedHeightVariationForBuilding
	Real m_minLowEnergyProductionSpeed;                    // +0xA74 MinLowEnergyProductionSpeed
	Real m_maxLowEnergyProductionSpeed;                    // +0xA78 MaxLowEnergyProductionSpeed
	Real m_lowEnergyPenaltyModifier;                       // +0xA7C LowEnergyPenaltyModifier
	Real m_multipleFactory;                                // +0xA80 MultipleFactory
	Real m_refundPercent;                                  // +0xA84 RefundPercent; percent
	UnsignedInt m_timeAfterDamageUntilRepairAllowed;       // +0xA88 TimeAfterDamageUntilRepairAllowed
	Real m_commandCenterHealRange;                         // +0xA8C CommandCenterHealRange
	Real m_commandCenterHealAmount;                        // +0xA90 CommandCenterHealAmount
	Int m_maxLineBuildObjects;                             // +0xA94 MaxLineBuildObjects
	Int m_maxTunnelCapacity;                               // +0xA98 MaxTunnelCapacity
	Real m_horizontalScrollSpeedFactor;                    // +0xA9C HorizontalScrollSpeedFactor
	Real m_verticalScrollSpeedFactor;                      // +0xAA0 VerticalScrollSpeedFactor
	Real m_screenEdgeScrollSpeedFactor;                    // +0xAA4 ScreenEdgeScrollSpeedFactor
	UnsignedInt m_screenEdgeScrollRampTime;                // +0xAA8 ScreenEdgeScrollRampTime; scanReal scaled then truncated, 0x0002F0F7
	Real m_scrollAmountCutoff;                             // +0xAAC ScrollAmountCutoff
	Real m_cameraAdjustSpeed;                              // +0xAB0 CameraAdjustSpeed
	Bool m_enforceMaxCameraHeight;                         // +0xAB4 EnforceMaxCameraHeight
	unsigned char m_unk0xAB5[0x3];						// +0xAB5 a byte the ctor 0x0023631C zeroes (Zero Hour's m_buildMapCache place)
	AsciiString m_unk0xAB8;                                // +0xAB8 the ctor zeroes +0xAB8/+0xABC/+0xAC0 and the dtor 0x002376CC destroys them (0x00036410) in reverse order
	AsciiString m_initialFile;                             // +0xABC (Zero Hour's order: m_initialFile, m_pendingFile)
	AsciiString m_pendingFile;                             // +0xAC0 RecorderClass::init 0x0037B326 hands it to GameInfo::setMap; playbackFile 0x0037D1E6 stores the replay's map
	unsigned char m_unk0xAC4[0x4];                         // +0xAC4 two bytes the ctor zeroes
	Int m_maxParticleCount;                                // +0xAC8 MaxParticleCount
	Int m_maxFieldParticleCount;                           // +0xACC MaxFieldParticleCount
	WeaponBonusSet * m_weaponBonus;                        // +0xAD0 WeaponBonus
	Real m_healthBonusRegular;	// +0xAD4 HealthBonus_Regular (ActiveBody::onVeterancyLevelChanged 0x004BE47D indexes it with the level)
	Real getHealthBonus(Int veterancyLevel) const { return (&m_healthBonusRegular)[veterancyLevel]; }
	Real m_healthBonusVeteran;                             // +0xAD8 HealthBonus_Veteran; percent
	Real m_healthBonusElite;                               // +0xADC HealthBonus_Elite; percent
	Real m_healthBonusHeroic;                              // +0xAE0 HealthBonus_Heroic; percent
	Real m_defaultStructureRubbleHeight;                   // +0xAE4 DefaultStructureRubbleHeight
	Real m_attributeModifierArmorMaxBonus;                 // +0xAE8 AttributeModifierArmorMaxBonus; percent
	AsciiString m_shellMapName;                            // +0xAEC ShellMapName
	Bool m_shellMapOn;                                     // +0xAF0 ShellMapOn
	Bool m_shellMapOffByCommandArgument;                   // +0xAF1 ShellMapOffByCommandArgument
	Bool m_playIntro;                                      // +0xAF2 PlayIntro
	unsigned char m_unk0xAF3[0x5];
	Real m_keyboardScrollSpeedFactor;                      // +0xAF8 KeyboardScrollSpeedFactor
	Real m_keyboardDefaultScrollSpeedFactor;               // +0xAFC KeyboardDefaultScrollSpeedFactor
	Bool m_animateWindows;                                 // +0xB00 AnimateWindows; the flag Shell's animation calls test
	Bool m_incrementalAGPBuf;                              // +0xB01 (Zero Hour's member order: m_animateWindows, m_incrementalAGPBuf, m_iniCRC, m_exeCRC; position only)
	unsigned char m_pad0xB02[2];
	UnsignedInt m_iniCRC;                                  // +0xB04 CRC of the INI files (LANAPI::RequestGameJoin 0x00449E9A copies it)
	Version m_exeCRC;                                      // +0xB08 the executable version record (0x30 bytes), read by the LAN join request
	UnsignedInt m_extraCRC;                                // +0xB38 the third CRC of the join request
	Int m_movementPenaltyDamageState;                      // +0xB3C MovementPenaltyDamageState; index into a name table
	Real m_damageRadiusMinimumForSplash;                   // +0xB40 DamageRadiusMinimumForSplash
	Int m_groupSelectMinSelectSize;                        // +0xB44 GroupSelectMinSelectSize
	Real m_groupSelectVolumeBase;                          // +0xB48 GroupSelectVolumeBase
	Real m_groupSelectVolumeIncrement;                     // +0xB4C GroupSelectVolumeIncrement
	Int m_maxUnitSelectSounds;                             // +0xB50 MaxUnitSelectSounds
	Real m_selectionFlashSaturationFactor;                 // +0xB54 SelectionFlashSaturationFactor
	Bool m_selectionFlashHouseColor;                       // +0xB58 SelectionFlashHouseColor
	Real m_cameraAudibleRadius;                            // +0xB5C CameraAudibleRadius
	Real m_groupMoveClickToGatherAreaFactor;               // +0xB60 GroupMoveClickToGatherAreaFactor
	unsigned char m_unk0xB64[0x4];
	// Defaults the OptionPreferences getters return when the key is absent (0x002E4BC7..0x002E4E12).
	Bool m_languageFilterPref;                             // +0xB68 LanguageFilter
	Bool m_turnOffMessengerInGame;                         // +0xB69 TurnOffMessengerInGame
	Bool m_doubleClickGuard;                               // +0xB6A DoubleClickGuard
	unsigned char m_unk0xB6B[0x1];
	Bool m_refreshNet;                                     // +0xB6C RefreshNet
	Bool m_displayForeignLanguage;                         // +0xB6D DisplayForeignLanguage
	unsigned char m_unk0xB6E[2];
	Bool m_saveCameraInReplay;                             // +0xB70 cleared by applyOptionPreferences 0x0023576A (Zero Hour's name)
	Bool m_useCameraInReplay;                              // +0xB71 UseCameraInReplay
	Real m_shakeSubtleIntensity;                           // +0xB74 ShakeSubtleIntensity
	Real m_shakeNormalIntensity;                           // +0xB78 ShakeNormalIntensity
	Real m_shakeStrongIntensity;                           // +0xB7C ShakeStrongIntensity
	Real m_shakeSevereIntensity;                           // +0xB80 ShakeSevereIntensity
	Real m_shakeCineExtremeIntensity;                      // +0xB84 ShakeCineExtremeIntensity
	Real m_shakeCineInsaneIntensity;                       // +0xB88 ShakeCineInsaneIntensity
	Real m_maxShakeIntensity;                              // +0xB8C MaxShakeIntensity
	Real m_maxShakeRange;                                  // +0xB90 MaxShakeRange
	Real m_sellPercentage;                                 // +0xB94 SellPercentage; percent
	Real m_baseRegenHealthPercentPerSecond;                // +0xB98 BaseRegenHealthPercentPerSecond; percent
	UnsignedInt m_baseRegenDelay;                          // +0xB9C BaseRegenDelay; duration, frames
	Int m_hotKeyTextColor;                                 // +0xBA0 HotKeyTextColor; parseColorInt
	AsciiString m_specialPowerViewObject;                  // +0xBA4 SpecialPowerViewObject
	_STL::vector<AsciiString> m_standardPublicBone;        // +0xBA8 StandardPublicBone
	Bool m_showMetrics;                                    // +0xBB4 ShowMetrics
	UnsignedInt m_defaultStartingCash;                     // +0xBB8 DefaultStartingCash
	bool m_networkDebugDisplayActive;		// +0xBBC set by Display::setDebugDisplayCallback 0x0025C6BD when the callback is NetworkDebugDisplay (role-inferred name)
	Bool m_runFast;		// +0xBBD ScriptEngine::isTimeFast 0x00203B47 answers true for it (when the game is not paused)
	unsigned char m_unk0xBBE[0x2];
	Int m_powerBarBase;                                    // +0xBC0 PowerBarBase
	Real m_powerBarIntervals;                              // +0xBC4 PowerBarIntervals
	Int m_powerBarYellowRange;                             // +0xBC8 PowerBarYellowRange
	Real m_gammaAdjustment;                                // +0xBCC the brightness preference mapped to 0.0..2.0 by applyOptionPreferences 0x0023576A
	UnsignedInt m_unlookPersistDuration;                   // +0xBD0 UnlookPersistDuration; duration, frames
	Bool m_updateImages;                                   // +0xBD4 set by parseUpdateImages 0x003B9A0C
	unsigned char m_unk0xBD5[7];
	RGBColor m_shroudColor;                               // +0xBDC ShroudColor
	UnsignedByte m_clearAlpha;                             // +0xBE8 ClearAlpha
	UnsignedByte m_fogAlpha;                               // +0xBE9 FogAlpha
	UnsignedByte m_shroudAlpha;                            // +0xBEA ShroudAlpha
	unsigned char m_unk0xBEB[0x1];
	RGBColor m_taintColor;                                // +0xBEC TaintColor
	RGBColor m_elvenWoodColor;                            // +0xBF8 ElvenWoodColor
	UnsignedByte m_taintAlpha;                             // +0xC04 TaintAlpha
	Int m_networkFPSHistoryLength;                         // +0xC08 NetworkFPSHistoryLength
	Int m_networkLatencyHistoryLength;                     // +0xC0C NetworkLatencyHistoryLength
	Int m_networkRunAheadMetricsTime;                      // +0xC10 NetworkRunAheadMetricsTime
	Int m_networkCushionHistoryLength;                     // +0xC14 NetworkCushionHistoryLength
	Int m_networkRunAheadSlack;                            // +0xC18 NetworkRunAheadSlack
	Int m_networkKeepAliveDelay;                           // +0xC1C NetworkKeepAliveDelay
	Int m_networkDisconnectTime;                           // +0xC20 NetworkDisconnectTime
	Int m_networkPlayerTimeoutTime;                        // +0xC24 NetworkPlayerTimeoutTime
	Int m_networkDisconnectScreenNotifyTime;               // +0xC28 NetworkDisconnectScreenNotifyTime
	Real m_keyboardCameraRotateSpeed;                      // +0xC2C KeyboardCameraRotateSpeed
	Int m_playStats;                                       // +0xC30 PlayStats
	UnsignedInt m_defaultVoiceAttackChargeTimeout;         // +0xC34 DefaultVoiceAttackChargeTimeout; duration, frames
	Real m_defaultMaxDistanceForEngaged;                   // +0xC38 DefaultMaxDistanceForEngaged
	UnsignedInt m_defaultEngagedStateTimeout;              // +0xC3C DefaultEngagedStateTimeout; duration, frames
	Int m_animationSharingCap;                             // +0xC40 AnimationSharingCap
	Real m_animationSharingFrameTolerance;                 // +0xC44 AnimationSharingFrameTolerance
	Real m_animationSharingSpeedTolerance;                 // +0xC48 AnimationSharingSpeedTolerance
	Real m_animationSharingWorryThreshold;                 // +0xC4C AnimationSharingWorryThreshold
	Real m_animationSharingDrasticThreshold;               // +0xC50 AnimationSharingDrasticThreshold
	_STL::vector<AsciiString> m_unk0xC54;				// +0xC54 the destructor destroys a vector of strings here
	unsigned char m_unk0xC60[0xA];
	Bool m_taintOn;                                        // +0xC6A TaintOn
	unsigned char m_unk0xC6B[0x99];
	AsciiString m_unk0xD04;								// +0xD04..+0xD3C: strings the destructor releases
	AsciiString m_unk0xD08;
	unsigned char m_unk0xD0C[0x18];
	Bool m_checkDuplicateCommandIDs;                       // +0xD24 set to 1 by the ctor 0x00236EB4; CommandIDHistory::accept 0x004D2164 skips the duplicate filter when clear
	unsigned char m_unk0xD25[0xB];
	AsciiString m_unk0xD30;
	unsigned char m_unk0xD34[0x4];
	AsciiString m_unk0xD38;
	AsciiString m_unk0xD3C;
	unsigned char m_unk0xD40[0x6];
	Bool m_unk0xD46;										// +0xD46 set by the command-line parser 0x003B9A29
	unsigned char m_unk0xD47[1];
	Int m_unk0xD48;										// +0xD48 set from the next argument by the command-line parser 0x003B9A3D
	AsciiString m_particleCursorAnim2DTemplateName;        // +0xD4C ParticleCursorAnim2DTemplateName
	UnsignedInt m_particleCursorBurstCount;                // +0xD50 ParticleCursorBurstCount
	GameClientRandomVariable m_particleCursorBurstFactor;  // +0xD54 ParticleCursorBurstFactor
	Real m_particleCursorStopBurstFactor;                  // +0xD60 ParticleCursorStopBurstFactor
	UnsignedInt m_particleCursorBurstFrequency;            // +0xD64 ParticleCursorBurstFrequency
	GameClientRandomVariable m_particleCursorParticleLife; // +0xD68 ParticleCursorParticleLife
	GameClientRandomVariable m_particleCursorSystemLife;   // +0xD74 ParticleCursorSystemLife
	GameClientRandomVariable m_particleCursorDriftVelX;    // +0xD80 ParticleCursorDriftVelX
	GameClientRandomVariable m_particleCursorDriftVelY;    // +0xD8C ParticleCursorDriftVelY
	GameClientRandomVariable m_particleCursorVelocityDrag; // +0xD98 ParticleCursorVelocityDrag
	GameClientRandomVariable m_particleCursorParticleSize; // +0xDA4 ParticleCursorParticleSize
	Bool m_particleCursorPerFrameSize;                     // +0xDB0 ParticleCursorPerFrameSize
	Int m_particleCursorAlpha;                             // +0xDB4 ParticleCursorAlpha
	ICoord2D m_particleCursorOffset;                       // +0xDB8 ParticleCursorOffset
	ICoord2D m_progressMovieOffset;                        // +0xDC0 ProgressMovieOffset
	ICoord2D m_progressMovieSize;                          // +0xDC8 ProgressMovieSize
	Bool m_useHelpTextSystem;                              // +0xDD0 UseHelpTextSystem
	Bool m_guiTextureFiltering;                            // +0xDD1 GuiTextureFiltering
	Real m_cameraLockHeightDelta;                          // +0xDD4 CameraLockHeightDelta
	Real m_cameraTerrainSampleRadiusForHeight;             // +0xDD8 CameraTerrainSampleRadiusForHeight
	Bool m_enableHouseColor;                               // +0xDDC EnableHouseColor
	Real m_cameraEaseFactor;                               // +0xDE0 CameraEaseFactor
	unsigned char m_unk0xDE4[0x8];
	Int m_goodCommandPoints[2];                            // +0xDEC GoodCommandPoints; two ints, parseCommandPoints 0x0023528E
	Int m_evilCommandPoints[2];                            // +0xDF4 EvilCommandPoints; two ints, parseCommandPoints 0x0023528E
	Int m_goodCommandPointsBonus;                          // +0xDFC GoodCommandPointsBonus
	Int m_evilCommandPointsBonus;                          // +0xE00 EvilCommandPointsBonus
	Int m_goodCommandPointsAI[2];                          // +0xE04 GoodCommandPointsAI; two ints, parseCommandPoints 0x0023528E
	Int m_evilCommandPointsAI[2];                          // +0xE0C EvilCommandPointsAI; two ints, parseCommandPoints 0x0023528E
	Int m_goodCommandPointsMP2[2];                         // +0xE14 GoodCommandPointsMP2; two ints, parseCommandPoints 0x0023528E
	Int m_evilCommandPointsMP2[2];                         // +0xE1C EvilCommandPointsMP2; two ints, parseCommandPoints 0x0023528E
	Int m_goodCommandPointsMP3[2];                         // +0xE24 GoodCommandPointsMP3; two ints, parseCommandPoints 0x0023528E
	Int m_evilCommandPointsMP3[2];                         // +0xE2C EvilCommandPointsMP3; two ints, parseCommandPoints 0x0023528E
	Int m_goodCommandPointsMP4[2];                         // +0xE34 GoodCommandPointsMP4; two ints, parseCommandPoints 0x0023528E
	Int m_evilCommandPointsMP4[2];                         // +0xE3C EvilCommandPointsMP4; two ints, parseCommandPoints 0x0023528E
	Int m_goodCommandPointsMP5[2];                         // +0xE44 GoodCommandPointsMP5; two ints, parseCommandPoints 0x0023528E
	Int m_evilCommandPointsMP5[2];                         // +0xE4C EvilCommandPointsMP5; two ints, parseCommandPoints 0x0023528E
	Int m_goodCommandPointsMP6[2];                         // +0xE54 GoodCommandPointsMP6 (also GoodCommandPointsMP56); two ints, parseCommandPoints 0x0023528E
	Int m_evilCommandPointsMP6[2];                         // +0xE5C EvilCommandPointsMP6 (also EvilCommandPointsMP56); two ints, parseCommandPoints 0x0023528E
	Int m_goodCommandPointsMP7[2];                         // +0xE64 GoodCommandPointsMP7; two ints, parseCommandPoints 0x0023528E
	Int m_evilCommandPointsMP7[2];                         // +0xE6C EvilCommandPointsMP7; two ints, parseCommandPoints 0x0023528E
	Int m_goodCommandPointsMP8[2];                         // +0xE74 GoodCommandPointsMP8 (also GoodCommandPointsMP78); two ints, parseCommandPoints 0x0023528E
	Int m_evilCommandPointsMP8[2];                         // +0xE7C EvilCommandPointsMP8 (also EvilCommandPointsMP78); two ints, parseCommandPoints 0x0023528E
	Int m_initialMaxRingLevel;                             // +0xE84 InitialMaxRingLevel
	Real m_resourceBonusMultiplier;                        // +0xE88 ResourceBonusMultiplier
	Int m_goodCommandPointLimit;                           // +0xE8C GoodCommandPointLimit
	Int m_evilCommandPointLimit;                           // +0xE90 EvilCommandPointLimit
	Int m_powerLimit;                                      // +0xE94 PowerLimit
	Real m_resourceMultiplierLimit;                        // +0xE98 ResourceMultiplierLimit
	Bool m_debugEmotionRanges;                             // +0xE9C draw the emotion/cheer-range circles (ActiveBody 0x004BF9ED, EmotionTrackerUpdate::update 0x004B1633)
	unsigned char m_unk0xE9D[0x13];
	ObjectFilter m_treeFadeObjectFilter;                   // +0xEB0 TreeFadeObjectFilter; iniParseObjectFilter 0x00361CA5; a 4-byte handle value, GlobalData() constructs it through 0x003623E5
	ObjectFilter m_camouflageDetectorObjectFilter;         // +0xEB4 CamouflageDetectorObjectFilter; iniParseObjectFilter 0x00361CA5
	ObjectFilter m_veterancyPipDrawObjectFilter;           // +0xEB8 VeterancyPipDrawObjectFilter; iniParseObjectFilter 0x00361CA5
	ObjectFilter m_victoryConditionStructureObjectFilter;  // +0xEBC VictoryConditionStructureObjectFilter; iniParseObjectFilter 0x00361CA5
	ObjectFilter m_victoryConditionUnitObjectFilter;       // +0xEC0 VictoryConditionUnitObjectFilter; iniParseObjectFilter 0x00361CA5
	MultiPlayMults m_multiPlayMults;                       // +0xEC4 MultiPlayMoneyMult (also MultiPlayUnitXPMult, MultiPlayBuildingXPMult, MultiPlayUnitSpeedMult, MultiPlayBuildingSpeedMult); the five parseMultiPlay*Mult parsers (MultiPlay*Mult tokens)
	Real m_handicapBuildSpeed5;                            // +0x10A4 HandicapBuildSpeed5; percent
	Real m_handicapBuildSpeed10;                           // +0x10A8 HandicapBuildSpeed10; percent
	Real m_handicapBuildSpeed15;                           // +0x10AC HandicapBuildSpeed15; percent
	Real m_handicapBuildSpeed20;                           // +0x10B0 HandicapBuildSpeed20; percent
	Real m_handicapBuildSpeed25;                           // +0x10B4 HandicapBuildSpeed25; percent
	Real m_handicapBuildSpeed30;                           // +0x10B8 HandicapBuildSpeed30; percent
	Real m_handicapBuildSpeed35;                           // +0x10BC HandicapBuildSpeed35; percent
	Real m_handicapBuildSpeed40;                           // +0x10C0 HandicapBuildSpeed40; percent
	Real m_handicapBuildSpeed45;                           // +0x10C4 HandicapBuildSpeed45; percent
	Real m_handicapBuildSpeed50;                           // +0x10C8 HandicapBuildSpeed50; percent
	Real m_handicapBuildSpeed55;                           // +0x10CC HandicapBuildSpeed55; percent
	Real m_handicapBuildSpeed60;                           // +0x10D0 HandicapBuildSpeed60; percent
	Real m_handicapBuildSpeed65;                           // +0x10D4 HandicapBuildSpeed65; percent
	Real m_handicapBuildSpeed70;                           // +0x10D8 HandicapBuildSpeed70; percent
	Real m_handicapBuildSpeed75;                           // +0x10DC HandicapBuildSpeed75; percent
	Real m_handicapBuildSpeed80;                           // +0x10E0 HandicapBuildSpeed80; percent
	Real m_handicapBuildSpeed85;                           // +0x10E4 HandicapBuildSpeed85; percent
	Real m_handicapBuildSpeed90;                           // +0x10E8 HandicapBuildSpeed90; percent
	Real m_handicapBuildSpeed95;                           // +0x10EC HandicapBuildSpeed95; percent
	Real m_handicapBuildSpeed100;                          // +0x10F0 HandicapBuildSpeed100; percent
	_STL::vector<AsciiString> m_scriptDebugFilter;	// +0x10F4 message prefixes the script debug window ignores
	Bool m_fastGamePlay;			// +0x1100 the game report lists it as -fastGamePlay
	unsigned char m_pad0x1101[3];	// +0x1101
	Int m_startingMoney;			// +0x1104 the game report lists it as -startingMoney when set
	Int m_blockedByWallLeeway;                             // +0x1108 BlockedByWallLeeway
	Real m_secondsBeforeBaseCheckActive;                   // +0x110C SecondsBeforeBaseCheckActive
	Bool m_unk0x1110;                                      // +0x1110 cleared by the command-line parser 0x003B9CDF
	Bool m_unk0x1111;                                      // +0x1111 set by 0x003B99AB
	unsigned char m_unk0x1112[2];
	TutorialInfo m_basicTutorial;							// +0x1114 BasicTutorial* tokens
	TutorialInfo m_advancedTutorial;						// +0x1138 AdvancedTutorial* tokens
	unsigned char m_unk0x115C[0xC];
	ObjectFilter m_objectsThatScore;                       // +0x1168 ObjectsThatScore; iniParseObjectFilter 0x00361CA5
	Real m_scoreKeeperUnitsBuiltMultiplier;                // +0x116C ScoreKeeper_UnitsBuiltMultiplier
	Real m_scoreKeeperUnitsDestroyedMultiplier;            // +0x1170 ScoreKeeper_UnitsDestroyedMultiplier
	Real m_scoreKeeperStructuresBuiltMultiplier;           // +0x1174 ScoreKeeper_StructuresBuiltMultiplier
	Real m_scoreKeeperStructuresDestroyedMultiplier;       // +0x1178 ScoreKeeper_StructuresDestroyedMultiplier
	Real m_scoreKeeperHeroesVettedMultiplier;              // +0x117C ScoreKeeper_HeroesVettedMultiplier
	Real m_scoreKeeperUnitsVettedMultiplier;               // +0x1180 ScoreKeeper_UnitsVettedMultiplier
	Real m_scoreKeeperObjectivesCompletedMultiplier;       // +0x1184 ScoreKeeper_ObjectivesCompletedMultiplier
	Real m_scoreKeeperSuppliesCollectedMultiplier;         // +0x1188 ScoreKeeper_SuppliesCollectedMultiplier
	Real m_scoreKeeperPowerPointsMultiplier;               // +0x118C ScoreKeeper_PowerPointsMultiplier
	Real m_scoreKeeperRegionCommandPointsMultiplier;       // +0x1190 ScoreKeeper_RegionCommandPointsMultiplier
	Real m_scoreKeeperRegionResourcesMultiplier;           // +0x1194 ScoreKeeper_RegionResourcesMultiplier
	Real m_scoreKeeperRegionPowerPointsMultiplier;         // +0x1198 ScoreKeeper_RegionPowerPointsMultiplier
	Real m_scoreKeeperTimeTakenMultiplier;                 // +0x119C ScoreKeeper_TimeTakenMultiplier
	Real m_scoreKeeperTimeTakenMaximumScore;               // +0x11A0 ScoreKeeper_TimeTakenMaximumScore
	Real m_scoreKeeperTimeTakenMinimumScore;               // +0x11A4 ScoreKeeper_TimeTakenMinimumScore
	Real m_scoreKeeperTotalVictoryRequiredScore;           // +0x11A8 ScoreKeeper_TotalVictoryRequiredScore
	Real m_scoreKeeperNormalVictoryRequiredScore;          // +0x11AC ScoreKeeper_NormalVictoryRequiredScore
	Real m_scoreKeeperNormalVictoryRequiredObjectivesPercentage;// +0x11B0 ScoreKeeper_NormalVictoryRequiredObjectivesPercentage
	Real m_scoreKeeperSkillPointsMultiplier;               // +0x11B4 ScoreKeeper_SkillPointsMultiplier
	Real m_scoreKeeperPlayerEliminatedMultiplier;          // +0x11B8 ScoreKeeper_PlayerEliminatedMultiplier
	unsigned char m_unk0x11BC[0x4];
	Int m_tintUnitIfPathingForMoreThan;                    // +0x11C0 TintUnitIfPathingForMoreThan
	Real m_clampedLOSHeightForCastleStructures;            // +0x11C4 ClampedLOSHeightForCastleStructures
	Bool m_unk0x11C8;                                      // +0x11C8 set by the command-line parser 0x003B9D5F
	unsigned char m_unk0x11C9[1];
	Bool m_planningModeEnabled;                            // +0x11CA PlanningModeEnabled
	UnsignedInt m_reinvisibityDelay;                       // +0x11CC ReinvisibityDelay; duration, frames
	Real m_invisibilityOpacityMin;                         // +0x11D0 InvisibilityOpacityMin
	Real m_invisibilityOpacityMax;                         // +0x11D4 InvisibilityOpacityMax
	UnsignedInt m_invisibilityOpacityCycleFrames;          // +0x11D8 InvisibilityOpacityCycleFrames
	UnsignedInt m_builderFadeOutTime;                      // +0x11DC BuilderFadeOutTime; duration, frames
	UnsignedInt m_builderFadeInTime;                       // +0x11E0 BuilderFadeInTime; duration, frames
	Real m_builderMoveFromNewStructureDistance;            // +0x11E4 BuilderMoveFromNewStructureDistance
	UnsignedInt m_maxPathfindCellsPerFrame;                // +0x11E8 MaxPathfindCellsPerFrame
	UnsignedInt m_maxCellsFindMeleeEngagementLocation;     // +0x11EC MaxCellsFindMeleeEngagementLocation
	UnsignedInt m_maxCellsAdjustDestination;               // +0x11F0 MaxCellsAdjustDestination
	UnsignedInt m_maxCellsAdjustHordeMeleeDestination;     // +0x11F4 MaxCellsAdjustHordeMeleeDestination
	UnsignedInt m_maxCellsAdjustTargetDestination;         // +0x11F8 MaxCellsAdjustTargetDestination
	UnsignedInt m_maxCellsAdjustToPossibleDestination;     // +0x11FC MaxCellsAdjustToPossibleDestination
	UnsignedInt m_maxCellsAdjustToMeleeDestination;        // +0x1200 MaxCellsAdjustToMeleeDestination
	UnsignedInt m_maxCellsAdjustToNearestGroundCell;       // +0x1204 MaxCellsAdjustToNearestGroundCell
	UnsignedInt m_maxCellsAdjustToNearestValidCell;        // +0x1208 MaxCellsAdjustToNearestValidCell
	UnsignedInt m_maxCellsPatchPath;                       // +0x120C MaxCellsPatchPath
	UnsignedInt m_maxCellsFindPathLimit;                   // +0x1210 MaxCellsFindPathLimit
	UnsignedInt m_maxCellsFindAttackPath;                  // +0x1214 MaxCellsFindAttackPath
	UnsignedInt m_maxCellsFindAttackPathSideways;          // +0x1218 MaxCellsFindAttackPathSideways
	UnsignedInt m_maxCellsToExamineTowardsGoal;            // +0x121C MaxCellsToExamineTowardsGoal
	Real m_maxCastleRadius;                                // +0x1220 MaxCastleRadius
	Real m_garrisonedRangeMultiplier;                      // +0x1224 GarrisonedRangeMultiplier
	Int m_fixedSeedOverride;	// +0x1228 InitRandom 0x00233F82 / InitGameLogicRandom 0x00233FC7 use it as the seed when it is not -1
	Int m_numMinutesBeforePlayersCanTransferMoney;         // +0x122C NumMinutesBeforePlayersCanTransferMoney
	UnsignedInt m_maxNumMembersToForceToImmediatelyEnter;  // +0x1230 MaxNumMembersToForceToImmediatelyEnter
	UnsignedInt m_waitToForceMemberToEnterDelay;           // +0x1234 WaitToForceMemberToEnterDelay
	unsigned char m_unk0x1238[0x8];
	AsciiString m_userDataDir;					// +0x1240 returned by 0x002360DE
	UnicodeString m_userDataDirW;				// +0x1244 returned by 0x002360FC
	AsciiString m_picturePath;					// +0x1248 the folder getPicturePath creates and caches
	AsciiString m_userDataLeafName;				// +0x124C UserDataLeafName
	GlobalData *m_next;					// +0x1250 the override chain reset 0x00235736 walks
};
// SIZE 0x1254
typedef char GlobalDataSize[(sizeof(GlobalData) == 0x1254) ? 1 : -1];
typedef char GlobalDataMultiPlayMults[(offsetof(GlobalData, m_multiPlayMults) == 0xEC4) ? 1 : -1];
typedef char GlobalDataTerrainLighting[(offsetof(GlobalData, m_terrainLighting) == 0x140 && sizeof(GlobalData::TerrainLighting) == 0x24) ? 1 : -1];
typedef char GlobalDataTerrainObjectsLighting[(offsetof(GlobalData, m_terrainObjectsLighting) == 0x3C8) ? 1 : -1];
typedef char GlobalDataTerrainLightPos[(offsetof(GlobalData, m_terrainAmbient) == 0x8D8 && offsetof(GlobalData, m_terrainLightPos) == 0x920) ? 1 : -1];
typedef char GlobalDataStrings[(offsetof(GlobalData, m_livingWorldCampaignOverrride) == 0x8C && offsetof(GlobalData, m_unk0xC54) == 0xC54 && offsetof(GlobalData, m_unk0xD04) == 0xD04 && offsetof(GlobalData, m_unk0xD30) == 0xD30 && offsetof(GlobalData, m_unk0xD3C) == 0xD3C) ? 1 : -1];
typedef char GlobalDataTutorials[(offsetof(GlobalData, m_scriptDebugFilter) == 0x10F4 && offsetof(GlobalData, m_basicTutorial) == 0x1114 && offsetof(GlobalData, m_advancedTutorial) == 0x1138 && offsetof(GlobalData, m_picturePath) == 0x1248) ? 1 : -1];
typedef char GlobalDataNext[(offsetof(GlobalData, m_next) == 0x1250) ? 1 : -1];

extern GlobalData *TheWritableGlobalData;

// The registry switch that makes the user map directory the local UserData\Maps.
Bool GetRegistryUseLocalUserMaps();

#endif
