// DRAFT: LivingWorldMapInfo from retail's field-parse table (VA 0x00BE4560+0x0).
// Offsets are compile-checked below; no function matches against it yet, so it stays
// in Draft until one does. Types are what each entry's parse function stores; names are
// the INI tokens.
// SIZE not established: members reach 0x1ED; bases and vptr, if any, are the leading gap.
// Unresolved members: m_ambient (+0x38, INI::parseRGBColor), m_sunRGB (+0x4C, INI::parseRGBColor), m_accent1RGB (+0x60, INI::parseRGBColor), m_accent2RGB (+0x74, INI::parseRGBColor), m_armyLineColorAttacking (+0xF0, INI::parseRGBColor), m_armyLineColorNeutral (+0xFC, INI::parseRGBColor), m_armyLineColorAllied (+0x108, INI::parseRGBColor), m_animRaysColor (+0x12C, INI::parseRGBColor), m_animRaysCreateSound (+0x148, INI::parseAudioEventRTS), m_battleMarkerCreatedSound (+0x14C, INI::parseAudioEventRTS), m_enterMapSound (+0x150, INI::parseAudioEventRTS), m_addShadowSubObject (+0x188, INI::parseAsciiStringVectorAppend), m_shadowColor (+0x1D0, INI::parseColorInt).
#pragma once
#ifndef BFME2_LIVING_WORLD_MAP_INFO_H
#define BFME2_LIVING_WORLD_MAP_INFO_H

#include <stddef.h>
#include "Common/GameType.h"
#include "Common/Coord2D.h"
#include "Common/Coord3D.h"
#include "Common/AsciiString.h"


class LivingWorldMapInfo
{
public:

	AsciiString m_mapObject;                               // +0x0 MapObject (EyeTower is a sub-block parsed by parseEyeTowerDefinition)
	Int m_numWorldTiles;                                   // +0x4 NumWorldTiles
	Coord2D m_center;                                      // +0x8 Center
	Coord2D m_extent;                                      // +0x10 Extent
	Coord2D m_aptCenter;                                   // +0x18 AptCenter
	Real m_aptZoom;                                        // +0x20 AptZoom
	Real m_aptPitch;                                       // +0x24 AptPitch
	Coord3D m_cloudPos;                                    // +0x28 CloudPos
	AsciiString m_embersPartSys;                           // +0x34 EmbersPartSys
	UnsignedByte m_ambient[0xC];                           // +0x38 Ambient; unresolved: INI::parseRGBColor
	Coord2D m_sunDir;                                      // +0x44 SunDir
	UnsignedByte m_sunRGB[0xC];                            // +0x4C SunRGB; unresolved: INI::parseRGBColor
	Coord2D m_accent1Dir;                                  // +0x58 Accent1Dir
	UnsignedByte m_accent1RGB[0xC];                        // +0x60 Accent1RGB; unresolved: INI::parseRGBColor
	Coord2D m_accent2Dir;                                  // +0x6C Accent2Dir
	UnsignedByte m_accent2RGB[0xC];                        // +0x74 Accent2RGB; unresolved: INI::parseRGBColor
	AsciiString m_menBanner;                               // +0x80 MenBanner
	AsciiString m_elvesBanner;                             // +0x84 ElvesBanner
	AsciiString m_dwarvesBanner;                           // +0x88 DwarvesBanner
	AsciiString m_isengardBanner;                          // +0x8C IsengardBanner
	AsciiString m_mordorBanner;                            // +0x90 MordorBanner
	AsciiString m_wildBanner;                              // +0x94 WildBanner
	AsciiString m_neutralBanner;                           // +0x98 NeutralBanner
	AsciiString m_menAnts;                                 // +0x9C MenAnts
	AsciiString m_elvesAnts;                               // +0xA0 ElvesAnts
	AsciiString m_dwarvesAnts;                             // +0xA4 DwarvesAnts
	AsciiString m_isengardAnts;                            // +0xA8 IsengardAnts
	AsciiString m_mordorAnts;                              // +0xAC MordorAnts
	AsciiString m_wildAnts;                                // +0xB0 WildAnts
	AsciiString m_neutralAnts;                             // +0xB4 NeutralAnts
	AsciiString m_battleMarker;                            // +0xB8 BattleMarker
	AsciiString m_palantirMarker;                          // +0xBC PalantirMarker
	AsciiString m_regionAwardDisputeMarker;                // +0xC0 RegionAwardDisputeMarker
	Real m_bannerScaleSpeed;                               // +0xC4 BannerScaleSpeed
	Real m_bannerMaxScale;                                 // +0xC8 BannerMaxScale
	Real m_bannerTiltAngle;                                // +0xCC BannerTiltAngle
	Real m_bannerHeight;                                   // +0xD0 BannerHeight
	Real m_armyHeight;                                     // +0xD4 ArmyHeight
	Real m_beaconHeight;                                   // +0xD8 BeaconHeight
	Real m_defaultArmyMoveSpeed;                           // +0xDC DefaultArmyMoveSpeed
	Real m_heroArmyIconDiameter;                           // +0xE0 HeroArmyIconDiameter
	UnsignedInt m_numPointsPerArmyLine;                    // +0xE4 NumPointsPerArmyLine
	Real m_armyLineHeightBias;                             // +0xE8 ArmyLineHeightBias
	Real m_armyLineWidth;                                  // +0xEC ArmyLineWidth
	UnsignedByte m_armyLineColorAttacking[0xC];            // +0xF0 ArmyLineColorAttacking; unresolved: INI::parseRGBColor
	UnsignedByte m_armyLineColorNeutral[0xC];              // +0xFC ArmyLineColorNeutral; unresolved: INI::parseRGBColor
	UnsignedByte m_armyLineColorAllied[0xC];               // +0x108 ArmyLineColorAllied; unresolved: INI::parseRGBColor
	AsciiString m_armyLineTextureName;                     // +0x114 ArmyLineTextureName
	AsciiString m_animRays;                                // +0x118 AnimRays
	AsciiString m_animRaysPartSys;                         // +0x11C AnimRaysPartSys
	Coord3D m_animRaysPartSysOffset;                       // +0x120 AnimRaysPartSysOffset
	UnsignedByte m_animRaysColor[0xC];                     // +0x12C AnimRaysColor; unresolved: INI::parseRGBColor
	Real m_animRaysColorScale;                             // +0x138 AnimRaysColorScale
	Int m_animRaysEffectShells;                            // +0x13C AnimRaysEffectShells
	Int m_animRaysEffectDiameter;                          // +0x140 AnimRaysEffectDiameter
	Int m_animRaysEffectLifetime;                          // +0x144 AnimRaysEffectLifetime
	UnsignedByte m_animRaysCreateSound[0x4];               // +0x148 AnimRaysCreateSound; unresolved: INI::parseAudioEventRTS
	UnsignedByte m_battleMarkerCreatedSound[0x4];          // +0x14C BattleMarkerCreatedSound; unresolved: INI::parseAudioEventRTS
	UnsignedByte m_enterMapSound[0x4];                     // +0x150 EnterMapSound; unresolved: INI::parseAudioEventRTS
	AsciiString m_animCloud;                               // +0x154 AnimCloud
	AsciiString m_animCloudPartSys;                        // +0x158 AnimCloudPartSys
	Int m_numAnimClouds;                                   // +0x15C NumAnimClouds
	Coord3D m_animCloudRegionMin;                          // +0x160 AnimCloudRegionMin
	Coord3D m_animCloudRegionMax;                          // +0x16C AnimCloudRegionMax
	Int m_animCloudLifetime;                               // +0x178 AnimCloudLifetime
	AsciiString m_mordorCloud;                             // +0x17C MordorCloud
	AsciiString m_cloudBorderSubObject;                    // +0x180 CloudBorderSubObject
	AsciiString m_textLayerSubObject;                      // +0x184 TextLayerSubObject
	UnsignedByte m_addShadowSubObject[0xC];                // +0x188 AddShadowSubObject; unresolved: INI::parseAsciiStringVectorAppend
	Coord3D m_cloudGrowthPos;                              // +0x194 CloudGrowthPos
	Int m_cloudGrowthRate;                                 // +0x1A0 CloudGrowthRate
	Real m_cloudInitialSize;                               // +0x1A4 CloudInitialSize
	Real m_cloudInitialOpacity;                            // +0x1A8 CloudInitialOpacity
	Real m_cloudGrowthSize;                                // +0x1AC CloudGrowthSize
	Bool m_isDefined;                                      // +0x1B0 set by parseLivingWorldMapInfoDefinition
	Real m_cameraBoundX;                                   // +0x1B4 CameraBoundX
	Real m_cameraBoundY;                                   // +0x1B8 CameraBoundY
	Real m_mouseWheelZoomPerTick;                          // +0x1BC MouseWheelZoomPerTick
	Real m_mouseWheelZoomDampenFactor;                     // +0x1C0 MouseWheelZoomDampenFactor
	Real m_autoScrollSpeed;                                // +0x1C4 AutoScrollSpeed
	Real m_maxAutoScrollTime;                              // +0x1C8 MaxAutoScrollTime
	Int m_clickScrollThreshold;                            // +0x1CC ClickScrollThreshold
	UnsignedByte m_shadowColor[0x4];                       // +0x1D0 ShadowColor; unresolved: INI::parseColorInt
	Int m_armySelectedFadeInStart;                         // +0x1D4 ArmySelectedFadeInStart
	Int m_armySelectedFadeInEnd;                           // +0x1D8 ArmySelectedFadeInEnd
	Int m_armySelectedFadeOutStart;                        // +0x1DC ArmySelectedFadeOutStart
	Int m_armySelectedFadeOutEnd;                          // +0x1E0 ArmySelectedFadeOutEnd
	Int m_armyHilightedFadeInTime;                         // +0x1E4 ArmyHilightedFadeInTime
	Int m_armyHilightedFadeOutTime;                        // +0x1E8 ArmyHilightedFadeOutTime
	Bool m_enableMapShadows;                               // +0x1EC EnableMapShadows
};
typedef char LivingWorldMapInfo_off_m_mapObject[(offsetof(LivingWorldMapInfo, m_mapObject) == 0x0) ? 1 : -1];
typedef char LivingWorldMapInfo_off_m_numWorldTiles[(offsetof(LivingWorldMapInfo, m_numWorldTiles) == 0x4) ? 1 : -1];
typedef char LivingWorldMapInfo_off_m_center[(offsetof(LivingWorldMapInfo, m_center) == 0x8) ? 1 : -1];
typedef char LivingWorldMapInfo_off_m_extent[(offsetof(LivingWorldMapInfo, m_extent) == 0x10) ? 1 : -1];
typedef char LivingWorldMapInfo_off_m_aptCenter[(offsetof(LivingWorldMapInfo, m_aptCenter) == 0x18) ? 1 : -1];
typedef char LivingWorldMapInfo_off_m_aptZoom[(offsetof(LivingWorldMapInfo, m_aptZoom) == 0x20) ? 1 : -1];
typedef char LivingWorldMapInfo_off_m_aptPitch[(offsetof(LivingWorldMapInfo, m_aptPitch) == 0x24) ? 1 : -1];
typedef char LivingWorldMapInfo_off_m_cloudPos[(offsetof(LivingWorldMapInfo, m_cloudPos) == 0x28) ? 1 : -1];
typedef char LivingWorldMapInfo_off_m_embersPartSys[(offsetof(LivingWorldMapInfo, m_embersPartSys) == 0x34) ? 1 : -1];
typedef char LivingWorldMapInfo_off_m_ambient[(offsetof(LivingWorldMapInfo, m_ambient) == 0x38) ? 1 : -1];
typedef char LivingWorldMapInfo_off_m_sunDir[(offsetof(LivingWorldMapInfo, m_sunDir) == 0x44) ? 1 : -1];
typedef char LivingWorldMapInfo_off_m_sunRGB[(offsetof(LivingWorldMapInfo, m_sunRGB) == 0x4C) ? 1 : -1];
typedef char LivingWorldMapInfo_off_m_accent1Dir[(offsetof(LivingWorldMapInfo, m_accent1Dir) == 0x58) ? 1 : -1];
typedef char LivingWorldMapInfo_off_m_accent1RGB[(offsetof(LivingWorldMapInfo, m_accent1RGB) == 0x60) ? 1 : -1];
typedef char LivingWorldMapInfo_off_m_accent2Dir[(offsetof(LivingWorldMapInfo, m_accent2Dir) == 0x6C) ? 1 : -1];
typedef char LivingWorldMapInfo_off_m_accent2RGB[(offsetof(LivingWorldMapInfo, m_accent2RGB) == 0x74) ? 1 : -1];
typedef char LivingWorldMapInfo_off_m_menBanner[(offsetof(LivingWorldMapInfo, m_menBanner) == 0x80) ? 1 : -1];
typedef char LivingWorldMapInfo_off_m_elvesBanner[(offsetof(LivingWorldMapInfo, m_elvesBanner) == 0x84) ? 1 : -1];
typedef char LivingWorldMapInfo_off_m_dwarvesBanner[(offsetof(LivingWorldMapInfo, m_dwarvesBanner) == 0x88) ? 1 : -1];
typedef char LivingWorldMapInfo_off_m_isengardBanner[(offsetof(LivingWorldMapInfo, m_isengardBanner) == 0x8C) ? 1 : -1];
typedef char LivingWorldMapInfo_off_m_mordorBanner[(offsetof(LivingWorldMapInfo, m_mordorBanner) == 0x90) ? 1 : -1];
typedef char LivingWorldMapInfo_off_m_wildBanner[(offsetof(LivingWorldMapInfo, m_wildBanner) == 0x94) ? 1 : -1];
typedef char LivingWorldMapInfo_off_m_neutralBanner[(offsetof(LivingWorldMapInfo, m_neutralBanner) == 0x98) ? 1 : -1];
typedef char LivingWorldMapInfo_off_m_menAnts[(offsetof(LivingWorldMapInfo, m_menAnts) == 0x9C) ? 1 : -1];
typedef char LivingWorldMapInfo_off_m_elvesAnts[(offsetof(LivingWorldMapInfo, m_elvesAnts) == 0xA0) ? 1 : -1];
typedef char LivingWorldMapInfo_off_m_dwarvesAnts[(offsetof(LivingWorldMapInfo, m_dwarvesAnts) == 0xA4) ? 1 : -1];
typedef char LivingWorldMapInfo_off_m_isengardAnts[(offsetof(LivingWorldMapInfo, m_isengardAnts) == 0xA8) ? 1 : -1];
typedef char LivingWorldMapInfo_off_m_mordorAnts[(offsetof(LivingWorldMapInfo, m_mordorAnts) == 0xAC) ? 1 : -1];
typedef char LivingWorldMapInfo_off_m_wildAnts[(offsetof(LivingWorldMapInfo, m_wildAnts) == 0xB0) ? 1 : -1];
typedef char LivingWorldMapInfo_off_m_neutralAnts[(offsetof(LivingWorldMapInfo, m_neutralAnts) == 0xB4) ? 1 : -1];
typedef char LivingWorldMapInfo_off_m_battleMarker[(offsetof(LivingWorldMapInfo, m_battleMarker) == 0xB8) ? 1 : -1];
typedef char LivingWorldMapInfo_off_m_palantirMarker[(offsetof(LivingWorldMapInfo, m_palantirMarker) == 0xBC) ? 1 : -1];
typedef char LivingWorldMapInfo_off_m_regionAwardDisputeMarker[(offsetof(LivingWorldMapInfo, m_regionAwardDisputeMarker) == 0xC0) ? 1 : -1];
typedef char LivingWorldMapInfo_off_m_bannerScaleSpeed[(offsetof(LivingWorldMapInfo, m_bannerScaleSpeed) == 0xC4) ? 1 : -1];
typedef char LivingWorldMapInfo_off_m_bannerMaxScale[(offsetof(LivingWorldMapInfo, m_bannerMaxScale) == 0xC8) ? 1 : -1];
typedef char LivingWorldMapInfo_off_m_bannerTiltAngle[(offsetof(LivingWorldMapInfo, m_bannerTiltAngle) == 0xCC) ? 1 : -1];
typedef char LivingWorldMapInfo_off_m_bannerHeight[(offsetof(LivingWorldMapInfo, m_bannerHeight) == 0xD0) ? 1 : -1];
typedef char LivingWorldMapInfo_off_m_armyHeight[(offsetof(LivingWorldMapInfo, m_armyHeight) == 0xD4) ? 1 : -1];
typedef char LivingWorldMapInfo_off_m_beaconHeight[(offsetof(LivingWorldMapInfo, m_beaconHeight) == 0xD8) ? 1 : -1];
typedef char LivingWorldMapInfo_off_m_defaultArmyMoveSpeed[(offsetof(LivingWorldMapInfo, m_defaultArmyMoveSpeed) == 0xDC) ? 1 : -1];
typedef char LivingWorldMapInfo_off_m_heroArmyIconDiameter[(offsetof(LivingWorldMapInfo, m_heroArmyIconDiameter) == 0xE0) ? 1 : -1];
typedef char LivingWorldMapInfo_off_m_numPointsPerArmyLine[(offsetof(LivingWorldMapInfo, m_numPointsPerArmyLine) == 0xE4) ? 1 : -1];
typedef char LivingWorldMapInfo_off_m_armyLineHeightBias[(offsetof(LivingWorldMapInfo, m_armyLineHeightBias) == 0xE8) ? 1 : -1];
typedef char LivingWorldMapInfo_off_m_armyLineWidth[(offsetof(LivingWorldMapInfo, m_armyLineWidth) == 0xEC) ? 1 : -1];
typedef char LivingWorldMapInfo_off_m_armyLineColorAttacking[(offsetof(LivingWorldMapInfo, m_armyLineColorAttacking) == 0xF0) ? 1 : -1];
typedef char LivingWorldMapInfo_off_m_armyLineColorNeutral[(offsetof(LivingWorldMapInfo, m_armyLineColorNeutral) == 0xFC) ? 1 : -1];
typedef char LivingWorldMapInfo_off_m_armyLineColorAllied[(offsetof(LivingWorldMapInfo, m_armyLineColorAllied) == 0x108) ? 1 : -1];
typedef char LivingWorldMapInfo_off_m_armyLineTextureName[(offsetof(LivingWorldMapInfo, m_armyLineTextureName) == 0x114) ? 1 : -1];
typedef char LivingWorldMapInfo_off_m_animRays[(offsetof(LivingWorldMapInfo, m_animRays) == 0x118) ? 1 : -1];
typedef char LivingWorldMapInfo_off_m_animRaysPartSys[(offsetof(LivingWorldMapInfo, m_animRaysPartSys) == 0x11C) ? 1 : -1];
typedef char LivingWorldMapInfo_off_m_animRaysPartSysOffset[(offsetof(LivingWorldMapInfo, m_animRaysPartSysOffset) == 0x120) ? 1 : -1];
typedef char LivingWorldMapInfo_off_m_animRaysColor[(offsetof(LivingWorldMapInfo, m_animRaysColor) == 0x12C) ? 1 : -1];
typedef char LivingWorldMapInfo_off_m_animRaysColorScale[(offsetof(LivingWorldMapInfo, m_animRaysColorScale) == 0x138) ? 1 : -1];
typedef char LivingWorldMapInfo_off_m_animRaysEffectShells[(offsetof(LivingWorldMapInfo, m_animRaysEffectShells) == 0x13C) ? 1 : -1];
typedef char LivingWorldMapInfo_off_m_animRaysEffectDiameter[(offsetof(LivingWorldMapInfo, m_animRaysEffectDiameter) == 0x140) ? 1 : -1];
typedef char LivingWorldMapInfo_off_m_animRaysEffectLifetime[(offsetof(LivingWorldMapInfo, m_animRaysEffectLifetime) == 0x144) ? 1 : -1];
typedef char LivingWorldMapInfo_off_m_animRaysCreateSound[(offsetof(LivingWorldMapInfo, m_animRaysCreateSound) == 0x148) ? 1 : -1];
typedef char LivingWorldMapInfo_off_m_battleMarkerCreatedSound[(offsetof(LivingWorldMapInfo, m_battleMarkerCreatedSound) == 0x14C) ? 1 : -1];
typedef char LivingWorldMapInfo_off_m_enterMapSound[(offsetof(LivingWorldMapInfo, m_enterMapSound) == 0x150) ? 1 : -1];
typedef char LivingWorldMapInfo_off_m_animCloud[(offsetof(LivingWorldMapInfo, m_animCloud) == 0x154) ? 1 : -1];
typedef char LivingWorldMapInfo_off_m_animCloudPartSys[(offsetof(LivingWorldMapInfo, m_animCloudPartSys) == 0x158) ? 1 : -1];
typedef char LivingWorldMapInfo_off_m_numAnimClouds[(offsetof(LivingWorldMapInfo, m_numAnimClouds) == 0x15C) ? 1 : -1];
typedef char LivingWorldMapInfo_off_m_animCloudRegionMin[(offsetof(LivingWorldMapInfo, m_animCloudRegionMin) == 0x160) ? 1 : -1];
typedef char LivingWorldMapInfo_off_m_animCloudRegionMax[(offsetof(LivingWorldMapInfo, m_animCloudRegionMax) == 0x16C) ? 1 : -1];
typedef char LivingWorldMapInfo_off_m_animCloudLifetime[(offsetof(LivingWorldMapInfo, m_animCloudLifetime) == 0x178) ? 1 : -1];
typedef char LivingWorldMapInfo_off_m_mordorCloud[(offsetof(LivingWorldMapInfo, m_mordorCloud) == 0x17C) ? 1 : -1];
typedef char LivingWorldMapInfo_off_m_cloudBorderSubObject[(offsetof(LivingWorldMapInfo, m_cloudBorderSubObject) == 0x180) ? 1 : -1];
typedef char LivingWorldMapInfo_off_m_textLayerSubObject[(offsetof(LivingWorldMapInfo, m_textLayerSubObject) == 0x184) ? 1 : -1];
typedef char LivingWorldMapInfo_off_m_addShadowSubObject[(offsetof(LivingWorldMapInfo, m_addShadowSubObject) == 0x188) ? 1 : -1];
typedef char LivingWorldMapInfo_off_m_cloudGrowthPos[(offsetof(LivingWorldMapInfo, m_cloudGrowthPos) == 0x194) ? 1 : -1];
typedef char LivingWorldMapInfo_off_m_cloudGrowthRate[(offsetof(LivingWorldMapInfo, m_cloudGrowthRate) == 0x1A0) ? 1 : -1];
typedef char LivingWorldMapInfo_off_m_cloudInitialSize[(offsetof(LivingWorldMapInfo, m_cloudInitialSize) == 0x1A4) ? 1 : -1];
typedef char LivingWorldMapInfo_off_m_cloudInitialOpacity[(offsetof(LivingWorldMapInfo, m_cloudInitialOpacity) == 0x1A8) ? 1 : -1];
typedef char LivingWorldMapInfo_off_m_cloudGrowthSize[(offsetof(LivingWorldMapInfo, m_cloudGrowthSize) == 0x1AC) ? 1 : -1];
typedef char LivingWorldMapInfo_off_m_cameraBoundX[(offsetof(LivingWorldMapInfo, m_cameraBoundX) == 0x1B4) ? 1 : -1];
typedef char LivingWorldMapInfo_off_m_cameraBoundY[(offsetof(LivingWorldMapInfo, m_cameraBoundY) == 0x1B8) ? 1 : -1];
typedef char LivingWorldMapInfo_off_m_mouseWheelZoomPerTick[(offsetof(LivingWorldMapInfo, m_mouseWheelZoomPerTick) == 0x1BC) ? 1 : -1];
typedef char LivingWorldMapInfo_off_m_mouseWheelZoomDampenFactor[(offsetof(LivingWorldMapInfo, m_mouseWheelZoomDampenFactor) == 0x1C0) ? 1 : -1];
typedef char LivingWorldMapInfo_off_m_autoScrollSpeed[(offsetof(LivingWorldMapInfo, m_autoScrollSpeed) == 0x1C4) ? 1 : -1];
typedef char LivingWorldMapInfo_off_m_maxAutoScrollTime[(offsetof(LivingWorldMapInfo, m_maxAutoScrollTime) == 0x1C8) ? 1 : -1];
typedef char LivingWorldMapInfo_off_m_clickScrollThreshold[(offsetof(LivingWorldMapInfo, m_clickScrollThreshold) == 0x1CC) ? 1 : -1];
typedef char LivingWorldMapInfo_off_m_shadowColor[(offsetof(LivingWorldMapInfo, m_shadowColor) == 0x1D0) ? 1 : -1];
typedef char LivingWorldMapInfo_off_m_armySelectedFadeInStart[(offsetof(LivingWorldMapInfo, m_armySelectedFadeInStart) == 0x1D4) ? 1 : -1];
typedef char LivingWorldMapInfo_off_m_armySelectedFadeInEnd[(offsetof(LivingWorldMapInfo, m_armySelectedFadeInEnd) == 0x1D8) ? 1 : -1];
typedef char LivingWorldMapInfo_off_m_armySelectedFadeOutStart[(offsetof(LivingWorldMapInfo, m_armySelectedFadeOutStart) == 0x1DC) ? 1 : -1];
typedef char LivingWorldMapInfo_off_m_armySelectedFadeOutEnd[(offsetof(LivingWorldMapInfo, m_armySelectedFadeOutEnd) == 0x1E0) ? 1 : -1];
typedef char LivingWorldMapInfo_off_m_armyHilightedFadeInTime[(offsetof(LivingWorldMapInfo, m_armyHilightedFadeInTime) == 0x1E4) ? 1 : -1];
typedef char LivingWorldMapInfo_off_m_armyHilightedFadeOutTime[(offsetof(LivingWorldMapInfo, m_armyHilightedFadeOutTime) == 0x1E8) ? 1 : -1];
typedef char LivingWorldMapInfo_off_m_enableMapShadows[(offsetof(LivingWorldMapInfo, m_enableMapShadows) == 0x1EC) ? 1 : -1];

#endif
