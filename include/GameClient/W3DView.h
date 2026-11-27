// Portions derived from Command & Conquer Generals and Command & Conquer Generals Zero Hour, (c) Electronic Arts Inc., GPL-3.0-or-later; see LICENSE and NOTICE.
// W3DView.h defines the W3D implementation of the tactical view (TheTacticalView, VA 0x00DFEA3C, points at the one instance). Class text and member names follow Zero Hour's
// W3DView.h where retail's layout agrees; BFME 2's additions keep offset names where no evidence names them.
//
// Target facts (retail game.dat 1.06, RVAs):
//  - Constructor 0x0008B7CF: View::View 0x0025EDE6 (View is 0xB4 bytes), then the 0x44-byte data record at +0xC0 (inline clear of +0xE8; a second
//    record of the same shape sits at +0x2478), then SubsystemInterface at +0xB4 (0x001B4E63), then the vftables 0x007C7568 (166 slots: View's 153
//    plus thirteen) and 0x007C7530 (SubsystemInterface's 14: the init / reset / dtor entries are `sub ecx,0xB4; jmp` thunks 0x0008DE53 / 0x0008BD4E / 0x0008BD43;
//    update 0x0008C3AC and draw 0x00087DA0 sit in it directly). Deleting destructor 0x0008DE5E, destructor 0x0008BD5E.
//  - The constructor's stores give the defaults named below (+0x10C 15, +0x110 9, +0x2408 10.0, +0x23D8 1, +0x2444 1.0, View +0x70 1.0).
//  - The camera-move records at +0x1AC / +0x1E0 / +0x208 are Zero Hour's TRotateCameraInfo / TPitchCameraInfo / TZoomCameraInfo (same field
//    positions: the flag byte follows each, at +0x1DC / +0x204 / +0x228); two more records of the same family (+0x22C, +0x258) and the 0x2074-byte
//    path state at +0x280 are BFME 2's.
//  - Slot order and slot targets are listed per declaration; an override keeps the name View.h gives that slot.
#pragma once
#ifndef BFME2_W3DVIEW_H
#define BFME2_W3DVIEW_H

#include <stddef.h>
#include <vector>
#include "GameClient/View.h"
#include "GameClient/ParabolicEase.h"
#include "GameClient/SmoothCameraHeightField.h"
#include "Common/SubsystemInterface.h"
#include "Common/AsciiString.h"
#include "Common/Coord2D.h"
#include "Common/ICoord2D.h"
#include "Common/Coord3D.h"
#include "Common/Coord3DBase.h"
#include "Common/Region2D.h"
#include "WWMath/Vector3.h"

class CameraClass;
struct Region3D;

// Zero Hour's camera-move records (W3DView.h). Retail's layouts are the same; each record's implicit constructor builds its ParabolicEase with (0, 0, 1).
struct TRotateCameraInfo
{
	Int numFrames;				// +0x00
	Int curFrame;				// +0x04
	Int startTimeMultiplier;	// +0x08
	Int endTimeMultiplier;		// +0x0C
	Int numHoldFrames;			// +0x10
	ParabolicEase ease;			// +0x14
	Bool trackObject;			// +0x1C
	// The rotation target uses the plain coordinate base inside the union.
	struct Target
	{
		ObjectID targetObjectID;
		Coord3DBase targetObjectPos;
	};
	struct Angle
	{
		Real startAngle;
		Real endAngle;
	};
	union
	{
		Target target;			// +0x20
		Angle angle;
	};
};
// SIZE 0x30
struct TPitchCameraInfo
{
	Int numFrames;				// +0x00
	Int curFrame;				// +0x04
	Real angle;					// +0x08
	Real startPitch;			// +0x0C
	Real endPitch;				// +0x10
	Int startTimeMultiplier;	// +0x14
	Int endTimeMultiplier;		// +0x18
	ParabolicEase ease;			// +0x1C
};
// SIZE 0x24
struct TZoomCameraInfo
{
	Int numFrames;				// +0x00
	Int curFrame;				// +0x04
	Real startZoom;				// +0x08
	Real endZoom;				// +0x0C
	Int startTimeMultiplier;	// +0x10
	Int endTimeMultiplier;		// +0x14
	ParabolicEase ease;			// +0x18
};
// SIZE 0x20

// BFME 2's fifth record: eight ints and a ParabolicEase. 0x00086D73 reads the frame counts at +0x00 / +0x04, the two angles in degrees at +0x10 / +0x14
// and the ease at +0x20; slot 60's setter (0x00088F38) fills it from the current field of view.
struct TFieldOfViewCameraInfo
{
	Int numFrames;				// +0x00
	Int curFrame;				// +0x04
	Int unk0x08;				// +0x08
	Int unk0x0C;				// +0x0C
	Real startAngle;			// +0x10 (degrees)
	Real endAngle;				// +0x14 (degrees)
	Int unk0x18;				// +0x18
	Int endTimeMultiplier;		// +0x1C  cameraModFinalTimeMultiplier sets the transition's final multiplier.
	ParabolicEase ease;			// +0x20
};
// SIZE 0x28

// One waypoint of the scripted camera path (0x14 bytes): the position, the waypoint's name and its id (slot 28 is told the id as the camera passes).
struct TCameraPathWaypoint
{
	Coord3D position;			// +0x00
	AsciiString name;			// +0x0C
	Int id;						// +0x10
};
// SIZE 0x14

// A screen-to-terrain cache element. screenToTerrain at 0x0008DE7A checks
// the screen coordinate, then copies the world coordinate and returns the
// hit byte. The 0x18 stride is also used by the reserve helper 0x0008B5B5.
struct TerrainPositionRequest
{
	ICoord2D screen;
	Coord3D world;
	Bool hit;
};

typedef char TerrainPositionRequestSize[(sizeof(TerrainPositionRequest) == 0x18) ? 1 : -1];

struct W3DViewEaseState
{
	W3DViewEaseState();
	UnsignedByte m_unk0x00[0x68];
	ParabolicEase m_ease;
};
typedef char W3DViewEaseStateSize[(sizeof(W3DViewEaseState) == 0x70) ? 1 : -1];

class W3DViewMotionState
{
public:
	W3DViewMotionState();
	virtual ~W3DViewMotionState();

	UnsignedByte m_unk0x04[0x28];
	std::vector<Int> m_samples;
	UnsignedByte m_unk0x38[0x28];
};
typedef char W3DViewMotionStateSize[(sizeof(W3DViewMotionState) == 0x60) ? 1 : -1];

// The camera's settings object (the member at +0x24C8 of W3DView; retail builds a global of the same class at VA 0x00BCF670 with constructor 0x00101C92 and vftable
// 0x007CF670, 22 slots). The map spells the class "Rva00BCF670CameraSettings"; the role names below are the map's own for the slots it names (the INI loaders
// loadMinHeight .. loadScrollSpeedScalar, reload, getCameraOffset, setTriggerArea, constrainPosition); slots 0 and 1 are the clamp bounds W3DView::setHeightAboveGround
// 0x0008D2B8 applies (slot 7 is the setter that W3DView::setDefaultView 0x00088D0A scales the maximum with). The other slots keep their slot numbers.
class CameraSettings
{
public:
	virtual Real getMinimum();								// slot 0, 0x00452D82
	virtual Real getMaximum();								// slot 1, 0x004F5F92
	virtual Real slot2();									// slot 2, 0x004F5F8A (a float getter)
	virtual Real slot3();									// slot 3, 0x005D870D (a float getter)
	virtual Real slot4();									// slot 4, 0x0049CB57 (a float getter)
	virtual Real slot5();									// slot 5, 0x00101CA0 (a float getter)
	virtual void slot6(Real value);							// slot 6, 0x00101CB0 (a float setter)
	virtual void setMaximum(Real value);					// slot 7, 0x00101CBE
	virtual void slot8(Real value);							// slot 8, 0x00101CCC (a float setter)
	virtual void slot9(Real value);							// slot 9, 0x00101CDA (a float setter)
	virtual void slot10(Real value);						// slot 10, 0x00101CE8 (a float setter)
	virtual void slot11(Bool value);						// slot 11, 0x00049F2B (a byte setter)
	virtual void loadMinHeight();							// slot 12, 0x00101A64
	virtual void loadMaxHeight();							// slot 13, 0x00101A9E
	virtual void loadPitchAngle();							// slot 14, 0x00101AD8
	virtual void loadYawAngle();							// slot 15, 0x00101B12
	virtual void loadScrollSpeedScalar();					// slot 16, 0x00101B4C
	virtual void reload();									// slot 17, 0x0010185A
	virtual void getCameraOffset(Coord3D *offset, Real *angle);	// slot 18, 0x001018CD
	virtual Bool setTriggerArea(const AsciiString &name, const Coord3D *position);	// slot 19, 0x00101B86
	virtual void slot20();									// slot 20, 0x002FD7D7
	virtual void constrainPosition(Coord3D *position);		// slot 21, 0x00101CF6
	void reset();											// 0x00101A20
	Real m_minHeight;										// +0x04 (loadMinHeight 0x00101A64; the fallback is GlobalData +0x9C)
	Real m_maxHeight;										// +0x08 (loadMaxHeight; GlobalData +0xA0)
	Real m_pitchAngle;										// +0x0C (loadPitchAngle; GlobalData +0xA4; reset stores 37.5)
	Real m_yawAngle;										// +0x10 (loadYawAngle; GlobalData +0xA8)
	Real m_scrollSpeedScalar;								// +0x14 (loadScrollSpeedScalar; GlobalData +0xAC; reset stores 1.0)
	Bool m_modified;										// +0x18 (reload 0x0010185A sets it when any of the four differs from reset's 37.5/0/120/300; slot 5 returns it as a float)
	void *m_unk0x1C;											// +0x1C (reset clears it)
	Coord3D m_lastPos;										// +0x20 (reset clears it: Zero Hour's Coord3D::zero())
};
// SIZE 0x2C
typedef char CameraSettingsSize[(sizeof(CameraSettings) == 0x2C) ? 1 : -1];

class W3DView : public View, public SubsystemInterface
{
public:
	W3DView();
	virtual ~W3DView();						// slot 0: deleting destructor 0x0008DE5E, destructor 0x0008BD5E

	// ---- View's slots that W3DView overrides, in slot order (names and signatures as View.h declares them) ----
	virtual void init();	// slot 4, 0x0008C224
	virtual void reset();	// slot 5, 0x00085947
	virtual Drawable * pickDrawable(const ICoord2D *screen, Bool forceAttack, Int pickType);	// slot 9, 0x0008B1E5
	virtual Int iterateDrawablesInRegion(IRegion2D *screenRegion, Bool (*callback)(Drawable *, void *), void *userData);	// slot 10, 0x00086245
	virtual void setWidth(Int width);	// slot 14, 0x00087B81
	virtual void setHeight(Int height);	// slot 16, 0x00087AF4
	virtual void setOrigin(Int x, Int y);	// slot 18, 0x00087C55
	virtual void forceRedraw();	// slot 20, 0x0008D134
	virtual void lookAt(const Coord3D *o);	// slot 21, 0x0008D55D
	virtual void initHeightForMap();	// slot 22, 0x0008D807
	virtual Int scrollBy(Coord2D *delta);	// slot 23, 0x0008CE2E
	virtual void moveCameraTo(const Coord3D *position, CameraMarker *marker, Int milliseconds, Int shutter, Real easeIn, Real easeOut);	// slot 24, 0x0008A2EF
	virtual void moveCameraAlongWaypointPath(Waypoint *way, Int milliseconds, Int shutter, Real easeIn, Real easeOut, Real unk, Int mode);	// slot 25, 0x00089E2A
	virtual void slot26(Int value);	// slot 26, 0x00086BC5
	virtual Bool slot27(Int waypointId);	// slot 27, 0x00086A82
	virtual void slot28(Int value);	// slot 28, 0x00087973
	virtual void slot29(Int value);	// slot 29, 0x0008BD36
	virtual Bool isCameraMovementFinished();	// slot 30, 0x00086A1E
	virtual void cameraModFinalZoom(Real finalZoom, Real easeIn, Real easeOut);	// slot 31, 0x000865DB
	virtual void cameraModRollingAverage(Int framesToAverage);	// slot 32, 0x0008690A
	virtual void cameraModFinalTimeMultiplier(Int finalMultiplier);	// slot 33, 0x00086812
	virtual void cameraModFinalPitch(Real finalPitch, Real easeIn, Real easeOut);	// slot 34, 0x0008691F
	virtual void cameraModFreezeTime();	// slot 35, 0x0008BC93
	virtual void cameraModFreezeAngle();	// slot 36, 0x00086702
	virtual void cameraModLookToward(Coord3D *pLoc);	// slot 37, 0x000890FE
	virtual void cameraModFinalLookToward(Coord3D *pLoc);	// slot 38, 0x000892F0
	virtual void cameraModFinalMoveTo(Coord3D *pLoc);	// slot 39, 0x00086761
	virtual void cameraEnableSlaveMode(const AsciiString &thingTemplateName, const AsciiString &boneName);	// slot 40, 0x000875A2
	virtual void cameraDisableSlaveMode();	// slot 41, 0x000875CE
	virtual void Add_Camera_Shake(const Coord3D &position, Real radius, Real duration, Real power);	// slot 42, 0x000875F2
	virtual void slot43();	// slot 43, 0x003EFCB9
	virtual Int getViewFilterType();	// slot 44, 0x002C8E91
	virtual Bool setViewFilterMode(Int filterMode);	// slot 45, 0x00085F2C
	virtual Bool setViewFilter(Int filter);	// slot 47, 0x00085F6A
	virtual void setFadeParameters(Int fadeFrames, Int direction);	// slot 48, 0x00085EC7
	virtual void set3DWireFrameMode(Bool enable);	// slot 49, 0x00085F05
	virtual void resetCamera(const Coord3D *position, Int milliseconds, Real easeIn, Real easeOut);	// slot 50, 0x0008D925
	virtual void rotateCamera(Real rotations, Int milliseconds, Bool stopAtEnd, Real easeIn, Real easeOut);	// slot 51, 0x0008647C
	virtual void rotateCameraTowardObject(ObjectID id, Int milliseconds, Int holdMilliseconds, Real easeIn, Real easeOut, Real unk);	// slot 52, 0x0008652B
	virtual void rotateCameraTowardPosition(const Coord3D *pLoc, Int milliseconds, Real easeIn, Real easeOut, Bool reverseRotation);	// slot 53, 0x00088D65
	virtual Bool isTimeFrozen();	// slot 54, 0x0008BC9B
	virtual Int getTimeMultiplier();	// slot 55, 0x0008BCA2
	virtual void setTimeMultiplier(Int multiple);	// slot 56, 0x0008BCA9
	virtual void setDefaultView(Real pitch, Real angle, Real maxHeight);	// slot 57, 0x00088D0A
	virtual Real slot58();	// slot 58, 0x0008615B: the camera settings' maximum
	virtual void slot59(Real value, Int milliseconds, Real easeIn, Real easeOut);	// slot 59, 0x00088EB4
	virtual void slot60(Real value, Int milliseconds, Real easeIn, Real easeOut);	// slot 60, 0x00088F38
	virtual void zoomCamera(Real finalZoom, Int milliseconds, Real easeIn, Real easeOut);	// slot 61, 0x00088FF6
	virtual void pitchCamera(Real finalPitch, Int milliseconds, Real easeIn, Real easeOut);	// slot 62, 0x0008907A
	virtual void setAngle(Real angle);	// slot 63, 0x0008D139
	virtual void setPitch(Real pitch);	// slot 65, 0x0008D1DD
	virtual void slot67(Real value);	// slot 67, 0x0008D218
	virtual void setAngleAndPitchToDefault();	// slot 69, 0x0008D253
	virtual void slot71();	// slot 71, 0x00087D42
	virtual void slot72();	// slot 72, 0x00087CF7
	virtual Real getZoom();	// slot 73, 0x000858E4
	virtual void setZoom(Real zoom);	// slot 74, 0x0008D3C1
	virtual void setHeightAboveGround(Real height);	// slot 76, 0x0008D2B8
	virtual void slot79();	// slot 79, 0x0008D477
	virtual WorldToScreenReturn worldToScreenTriReturn(const Coord3D *w, ICoord2D *s);	// slot 88, 0x0008619F
	virtual void slot89();	// slot 89, 0x0050B238
	virtual Bool screenToTerrain(const ICoord2D *screen, Coord3D *world, Bool flag);	// slot 90, 0x0008DE7A
	virtual void screenToWorldAtZ(const ICoord2D *screen, Coord3D *world, Real z);	// slot 91, 0x0008A0E5
	virtual void slot94();	// slot 94, 0x0008605A
	virtual void slot95();	// slot 95, 0x00085B36
	virtual void slot97(ObjectID id);	// slot 97, 0x00086065
	virtual void slot99(Int mode, Real value);	// slot 99, 0x00086093
	virtual void slot104();	// slot 104, 0x00088D05
	virtual void shake(const Coord3D *epicenter, CameraShakeType type);	// slot 107, 0x00087452
	virtual void slot110();	// slot 110, 0x0008BD59
	virtual void slot111(Coord2D *value);	// slot 111, 0x0008BCB6
	virtual Coord2D *slot112();	// slot 112, 0x0008BCC8
	virtual void slot113();	// slot 113, 0x0008A566
	virtual void slot114(const AsciiString &markerName);	// slot 114, 0x0008AD96
	virtual Bool slot115();	// slot 115, 0x0008BCCC
	virtual void slot116();	// slot 116, 0x0008ADEA
	virtual void slot119();	// slot 119, 0x00087642
	virtual void slot125();	// slot 125, 0x00087952
	virtual void slot126();	// slot 126, 0x0008BD27
	virtual Bool slot130();	// slot 130, 0x0008BCF4
	virtual void slot131(Bool value);	// slot 131, 0x0008BCE7
	virtual Bool slot137(const AsciiString &name);	// slot 137, 0x000879A9: asks the camera settings to move to the named trigger area
	virtual void slot138();	// slot 138, 0x0008BD2B
	virtual Real slot143();	// slot 143, 0x000879E6
	virtual Real slot144();	// slot 144, 0x000879ED
	virtual Real slot145();	// slot 145, 0x000879F4
	virtual Bool slot146();	// slot 146, 0x000879FB
	virtual void slot147(Bool flag);	// slot 147, 0x00087A02
	virtual void slot148(Bool flag);	// slot 148, 0x00087A0F
	virtual void slot149(Bool flag);	// slot 149, 0x00087A1C
	virtual void slot150(Bool flag);	// slot 150, 0x00087A29

	// SubsystemInterface's update and draw (the +0xB4 table entries 0x0008C3AC and 0x00087DA0).
	virtual void update();
	virtual void draw();

	// Non-virtual members; Zero Hour's W3DView keeps them private and the map's names carry that (AAE).
private:
	void setCameraTransform();
	void constrainPosition(Coord3D *position);	// 0x000860AF
	Real *cancelCameraMovements();			// 0x000869CF: stops every running camera record and the scripted lock; returns the angle triple at +0x244C
	void getPickRay(const ICoord2D *screen, Vector3 *rayStart, Vector3 *rayEnd);
	void moveAlongWaypointPath(Int milliseconds);	// (Zero Hour's name)
	void rotateCameraOneFrame();	// (Zero Hour's name)
	void calcCameraConstraints();			// Recomputes m_cameraConstraint from the map extent and how far the view reaches ahead of the look-at point
	void getAxisAlignedViewRegion(Region3D &axisAlignedRegion);	// The world-space box around the visible corners, widened by the overscan and the guard band
	void zoomCameraOneFrame();				// 0x00086CDA: advances the record at +0x208 and writes View's zoom (+0x3C); Zero Hour's name
	void pitchCameraOneFrame();				// 0x00086E24: advances the record at +0x1E0 and writes View's +0x70 (Zero Hour's m_FXPitch); Zero Hour's name
	void pitchOneFrame();					// 0x00086EBD: advances the record at +0x258 and writes View's +0x30; called by pitchCamera 0x0008907A for a one-millisecond move
	void fieldOfViewCameraOneFrame();		// 0x00086D73: advances the record at +0x22C and writes the field of view (+0x6C) in radians
public:
	void calcDeltaScroll(Real *screenDelta);	// The screen-space movement of the look-at point since the last frame (the view filters read it)

	// ---- slots 153..165, W3DView's own ----
	virtual void slot153(Int);				// slot 153, 0x0047A69C (the shared `ret 4` stub)
	virtual void cameraEnableRealZoomMode();	// slot 154, 0x000875D6: sets +0x243A and View +0x70 (1.0), then calls slot 95; Zero Hour's cameraEnableRealZoomMode
	virtual void cameraDisableRealZoomMode();	// slot 155, 0x0008DAE4: the mirror of slot 154; Zero Hour's cameraDisableRealZoomMode
	virtual Real slot156(Real value);		// slot 156, 0x00086166
	virtual void setViewFilterPos(const Coord3D *pos);	// slot 157, 0x00085F12; Zero Hour's setViewFilterPos
	virtual void slot158();					// slot 158, 0x0008DB18
	virtual void slot159(Int value);		// slot 159, 0x0008780A
	virtual void slot160(Bool value);		// slot 160, 0x0008BCD3: stores the byte at +0x2490
	virtual Bool slot161();					// slot 161, 0x0008BCE0: returns the byte at +0x2490
	virtual void slot162(Bool value);		// slot 162, 0x0008BCFB: stores the byte at +0x249E
	virtual Bool slot163();					// slot 163, 0x0008BD08: returns the byte at +0x249E
	virtual void slot164(Bool value);		// slot 164, 0x0008BD13: stores the byte at +0x2448
	virtual Bool slot165();					// slot 165, 0x0008BD20: returns the byte at +0x2448

public:
	// +0x0C0: the 0x44-byte data record (retail's constructor clears the int at +0xE8 inline).
	UnsignedInt m_unk0xC0[0x11];			// +0x0C0
	CameraClass *m_3DCamera;				// +0x104 (0 in the constructor)
	CameraClass *m_2DCamera;				// +0x108 (0; the destructor 0x0008BD5E releases it)
	Int m_viewFilterMode;					// +0x10C 15
	Int m_viewFilter;						// +0x110 9
	Bool m_isWireFrameEnabled;				// +0x114
	Bool m_nextWireFrameEnabled;			// +0x115 (slot 49's setter)
	Coord2D m_shakeOffset;					// +0x118
	Real m_shakeAngleCos;					// +0x120
	Real m_shakeAngleSin;					// +0x124
	Real m_shakeIntensity;					// +0x128
	Vector3 m_shakerAngles;					// +0x12C
	Real m_unk0x138;						// +0x138 (0 in the constructor)
	W3DViewEaseState m_easeState;

	TRotateCameraInfo m_rcInfo;				// +0x1AC
	Bool m_doingRotateCamera;				// +0x1DC
	TPitchCameraInfo m_pcInfo;				// +0x1E0
	Bool m_doingPitchCamera;				// +0x204
	TZoomCameraInfo m_zcInfo;				// +0x208
	Bool m_doingZoomCamera;					// +0x228
	TFieldOfViewCameraInfo m_fovInfo;		// +0x22C: BFME 2's fifth camera-move record (built by 0x0008589E); 0x00086D73 advances it
	Bool m_doingFovCamera;           // +0x254  The field-of-view step clears this flag when the animation finishes.
	TPitchCameraInfo m_scriptedPitchInfo; // +0x258  pitchCamera initializes this transition and pitchOneFrame advances it.
	Bool m_doingScriptedPitch;       // +0x27C  pitchCamera sets this flag and pitchOneFrame clears it on completion.
	Bool m_doingScriptedCameraLock;			// +0x27D (setCameraLock 0x00086065 clears it, setSnapMode 0x00086093 sets it)
	Bool m_CameraArrivedAtWaypointOnPathFlag;	// +0x27E
	UnsignedInt m_unk0x280;					// +0x280: the waypoint-path state starts here: a vftable pointer (0x2074 bytes, built by 0x00089971)
	Int m_pathTotalMilliseconds;			// +0x284 (cameraModFinalPitch 0x0008691F subtracts the elapsed time from it)
	Int m_pathElapsedMilliseconds;			// +0x288
	Int m_unk0x28C;							// +0x28C
	ParabolicEase m_pathEase;				// +0x290
	Real m_unk0x298;						// +0x298
	Real m_unk0x29C;						// +0x29C
	Int m_unk0x2A0;							// +0x2A0
	Bool m_unk0x2A4;						// +0x2A4
	Int m_rollingAverageFrames;				// +0x2A8 (cameraModRollingAverage 0x0008690A; Zero Hour: rollingAverageFrames)
	TCameraPathWaypoint m_pathWaypoints[255];	// +0x2AC
	TCameraPathWaypoint m_unk0x1698[4];		// +0x1698
	Real m_pathCameraAngles[255];			// +0x16E8 (cameraModFreezeAngle 0x00086702 copies the first into the rest)
	Real m_pathSegmentLengths[257];			// +0x1AE4
	Real m_pathTotalDistance;				// +0x1EE8
	Real m_pathGroundStart;					// +0x1EEC
	Real m_pathGroundEnd;					// +0x1EF0
	Int m_pathTimeMultipliers[255];			// +0x1EF4
	Int m_pathNumValues;					// +0x22F0
	W3DViewMotionState m_motionStateA;
	Int m_cameraMovementMode;				// +0x2354
	Int m_unk0x2358;						// +0x2358
	Int m_unk0x235C;						// +0x235C
	Int m_unk0x2360;						// +0x2360
	Int m_unk0x2364;						// +0x2364
	W3DViewMotionState m_motionStateB;
	Bool m_unk0x23C8;						// +0x23C8
	Int m_currentCameraWaypointID;   // +0x23CC  The waypoint test compares against this ID, and slot 28 notifies the message stream when it changes.
	Bool m_freezeTimeForCameraMovement;		// +0x23D0 (slot 35 sets it, slot 54 reads it)
	Int m_timeMultiplier;					// +0x23D4 (slots 55 / 56; the rotate, pitch and zoom setters copy it into their time-multiplier fields)
	Bool m_cameraHasMovedSinceRequest;		// +0x23D8 1
	std::vector<TerrainPositionRequest> m_locationRequests;	// +0x23DC
	Coord3D m_cameraOffset;					// +0x23E8
	Coord2D m_unk0x23F4;					// +0x23F4
	Coord2D m_scrollAmount;					// +0x23FC
	Real m_scrollAmountCutoff;				// +0x2404
	Real m_groundLevel;						// +0x2408 10.0
	Region2D m_cameraConstraint;			// +0x240C
	Bool m_cameraConstraintValid;			// +0x241C
	Coord3D m_unk0x2420;					// +0x2420
	Coord3D m_unk0x242C;					// +0x242C
	Bool m_stopRotationAtEnd;        // +0x2438  rotateCamera stores its stopAtEnd argument here.
	Bool m_isCameraSlaved;					// +0x2439 (cameraDisableSlaveMode clears it)
	Bool m_useRealZoomCam;					// +0x243A (cameraEnableRealZoomMode sets it)
	AsciiString m_cameraSlaveObjectName;	// +0x243C
	AsciiString m_cameraSlaveObjectBoneName;	// +0x2440
	Real m_unk0x2444;						// +0x2444 1.0
	Bool m_unk0x2448;						// +0x2448 (slots 164 / 165)
	Bool m_unk0x2449;						// +0x2449 (slot 115 reads it)
	Coord3D m_unk0x244C;					// +0x244C
	SmoothCameraHeightField m_smoothCameraHeightField;
	UnsignedInt m_unk0x2478[6];				// +0x2478: a second 0x44-byte data record (to +0x24BC), the same shape as the one at +0xC0
	Bool m_unk0x2490;						// +0x2490 (slots 160 / 161)
	Bool m_unk0x2491;						// +0x2491
	Bool m_unk0x2492;						// +0x2492
	Bool m_unk0x2493;						// +0x2493
	UnsignedInt m_unk0x2494[2];				// +0x2494
	Bool m_unk0x249C;						// +0x249C
	Bool m_unk0x249D;						// +0x249D (slots 130 / 131)
	Bool m_unk0x249E;						// +0x249E (slots 162 / 163)
	Bool m_unk0x249F;						// +0x249F
	Int m_unk0x24A0;						// +0x24A0
	UnsignedInt m_unk0x24A4[6];				// +0x24A4
	Int m_unk0x24BC;						// +0x24BC
	Bool m_unk0x24C0;						// +0x24C0
	Int m_unk0x24C4;						// +0x24C4
	CameraSettings m_cameraSettings;		// +0x24C8 (slot 17 reloads, slot 18 reads the offset and angle)
	Real m_unk0x24F4;						// +0x24F4 (slot 143)
	Real m_angleTravelled;					// +0x24F8 (slot 144; setAngle adds the absolute angle change to it)
	Real m_heightTravelled;					// +0x24FC (slot 145; setHeightAboveGround adds the absolute height change to it)
	Bool m_unk0x2500;						// +0x2500 (slot 146)
	Bool m_unk0x2501;						// +0x2501 (slot 149 stores it)
	Bool m_cameraAngleLocked;        // +0x2502  setAngle exits when this flag is set.
	Bool m_cameraHeightLocked;       // +0x2503  The matched setHeightAboveGround body exits when this flag is set.
	Bool m_unk0x2504;						// +0x2504 (slot 150 stores it)
};
// SIZE 0x2508
typedef char TCameraPathWaypointSize[(sizeof(TCameraPathWaypoint) == 0x14) ? 1 : -1];
typedef char W3DViewSize[(sizeof(W3DView) == 0x2508) ? 1 : -1];
typedef char W3DViewPathAnglesOffset[(offsetof(W3DView, m_pathCameraAngles) == 0x16E8) ? 1 : -1];
typedef char W3DViewPathCountOffset[(offsetof(W3DView, m_pathNumValues) == 0x22F0) ? 1 : -1];
typedef char W3DViewRecordOffset[(offsetof(W3DView, m_unk0xC0) == 0xC0) ? 1 : -1];
typedef char W3DViewRcInfoOffset[(offsetof(W3DView, m_rcInfo) == 0x1AC) ? 1 : -1];
typedef char W3DViewZcInfoOffset[(offsetof(W3DView, m_zcInfo) == 0x208) ? 1 : -1];
typedef char W3DViewGroundLevelOffset[(offsetof(W3DView, m_groundLevel) == 0x2408) ? 1 : -1];
typedef char W3DViewCameraConstraintOffset[(offsetof(W3DView, m_cameraConstraint) == 0x240C) ? 1 : -1];
typedef char W3DViewNameOffset[(offsetof(W3DView, m_cameraSlaveObjectName) == 0x243C) ? 1 : -1];
typedef char W3DViewTailOffset[(offsetof(W3DView, m_unk0x24F4) == 0x24F4) ? 1 : -1];
typedef char W3DViewEaseStateOffset[(offsetof(W3DView, m_easeState) == 0x13C) ? 1 : -1];
typedef char W3DViewMotionAOffset[(offsetof(W3DView, m_motionStateA) == 0x22F4) ? 1 : -1];
typedef char W3DViewMotionBOffset[(offsetof(W3DView, m_motionStateB) == 0x2368) ? 1 : -1];
typedef char W3DViewSmoothHeightOffset[(offsetof(W3DView, m_smoothCameraHeightField) == 0x2458) ? 1 : -1];

#endif
