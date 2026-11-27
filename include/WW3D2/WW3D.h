// Portions derived from Command & Conquer Generals and Command & Conquer Generals Zero Hour, (c) Electronic Arts Inc., GPL-3.0-or-later; see LICENSE and NOTICE.
// WW3D.h: WW3D's static engine interface (no instance data), declared in Zero Hour's order.
// Target facts: CameraClass::Device_To_View_Space 0x00134650 calls Get_Render_Target_Resolution 0x00116FF0
// with four out-parameters (width, height, bit depth, windowed) and converts the device point with the first two.
// Retail differs from Zero Hour in these recorded ways: the render-device functions return bool where Zero Hour
// returns WW3DErrorType (the map spells them SA_N...: Set_Any_Render_Device 0x00116F00, Set_Render_Device
// 0x00116F10, Set_Next_Render_Device 0x00116F50, Set_Device_Resolution 0x00116FC0); each forwards to the
// DXWrapper function of the same name (a friend of this class) and converts the result with `setne`.
// The data identities below are the data ledger's (?Name@WW3D@@0..., old-ledger names, unproven).
#pragma once
#ifndef BFME2_WW3D_H
#define BFME2_WW3D_H

class FrameGrabClass;
class StaticSortListClass;
class RenderObjClass;
class RenderInfoClass;

class WW3D
{
public:
	enum PrelitModeEnum {
		PRELIT_MODE_VERTEX,
		PRELIT_MODE_LIGHTMAP_MULTI_PASS,
		PRELIT_MODE_LIGHTMAP_MULTI_TEXTURE
	};
	enum NPatchesGapFillingModeEnum {
		NPATCHES_GAP_FILLING_DISABLED,
		NPATCHES_GAP_FILLING_ENABLED,
		NPATCHES_GAP_FILLING_FORCE
	};

	static bool Set_Render_Device(int dev = -1, int resx = -1, int resy = -1, int bits = -1, int windowed = -1, bool resize_window = false, bool reset_device = false, bool restore_assets = true);	// 0x00116F10
	static bool Set_Next_Render_Device(void);					// 0x00116F50
	static bool Set_Any_Render_Device(void);					// 0x00116F00
	static void Get_Render_Target_Resolution(int &set_w, int &set_h, int &set_bits, bool &set_windowed);	// 0x00116FF0
	static bool Set_Device_Resolution(int w = -1, int h = -1, int bits = -1, int windowed = -1, bool resize_window = false);	// 0x00116FC0

	static void Sync(unsigned int sync_time);					// 0x00117490
	static void Normalize_Coordinates(int x, int y, float &fx, float &fy);	// 0x001184D0: pixel coordinates to 0..1 screen coordinates
	// 0 = bilinear, 1 = trilinear, 2 = anisotropic
	static void Set_Texture_Filter(int filter);					// 0x00117280
	static int Get_Texture_Filter() { return TextureFilter; }
	static unsigned int Get_Sync_Time(void) { return SyncTime; }	// the frame clock animations step from (Animatable3DObjClass::Set_Animation 0x001A4BF0 and Single_Anim_Progress 0x001A5320 read it)
	// The time between the previous sync and this one.
	static unsigned int Get_Frame_Time(void) { return SyncTime - PreviousSyncTime; }
	// DX8MeshRendererClass::Request_Log_Statistics 0x00143D00 reads it.
	static int Get_Frame_Count(void) { return FrameCount; }
	static bool Is_Rendering(void) { return IsRendering; }
	static bool End_Render(bool flip_frame = true);				// 0x00117410

	static void Start_Movie_Capture(const char *filename_base = "Movie", float frame_rate = 15, int buffer_count = 16, bool compress = false);	// 0x00118530 (the last two parameters: structural names)
	static void Stop_Movie_Capture(void);						// 0x00117A10
	static bool Toggle_Movie_Capture(const char *filename_base = "Movie", float frame_rate = 15, int buffer_count = 16, bool compress = false);	// 0x00118610
	static void Pause_Movie(bool mode);							// 0x00117A70
	static bool Is_Movie_Paused();								// 0x00117A80
	static bool Is_Recording_Next_Frame();						// 0x00117A90
	static bool Is_Movie_Ready();								// 0x00117AC0
	static void Capture_Next_Movie_Frame();						// 0x00117A60

	static void *Get_Window(void);								// 0x00116F60
	static void _Invalidate_Mesh_Cache(void);					// 0x001170A0
	static void _Invalidate_Textures(void);						// 0x001171B0
	static void Set_Texture_Reduction(int value);				// 0x00117BB0 (retail has no minimum-dimension argument)
	static int Get_Texture_Reduction(void);						// 0x00117C00
	static void Set_Gamma(float gamma, float bright, float contrast, bool calibrate = true);	// 0x00117CD0
	static void Render_And_Clear_Static_Sort_Lists(RenderInfoClass &rinfo);	// 0x00117C60
	static void Override_Current_Static_Sort_Lists(StaticSortListClass *sort_list);	// 0x00117CB0
	static void Reset_Current_Static_Sort_Lists_To_Default(void);	// 0x00117CC0
	static void Enable_Sorting(bool onoff);						// 0x00117C90
	static void Add_To_Static_Sort_List(RenderObjClass *robj, unsigned int sort_level);	// 0x00117C40
	static bool Is_Sorting_Enabled(void) { return IsSortingEnabled; }
	static void Set_Prelit_Mode(PrelitModeEnum mode) { PrelitMode = mode; }
	static PrelitModeEnum Get_Prelit_Mode() { return PrelitMode; }
	static void Set_NPatches_Gap_Filling_Mode(NPatchesGapFillingModeEnum mode);	// 0x00116E50
	static NPatchesGapFillingModeEnum Get_NPatches_Gap_Filling_Mode() { return NPatchesGapFillingMode; }
	static void Set_NPatches_Level(unsigned level);				// 0x00116E70
	static unsigned Get_NPatches_Level() { return NPatchesLevel; }
	static void Enable_Texturing(bool b);						// 0x00117BD0
	static bool Is_Texturing_Enabled() { return IsTexturingEnabled; }
	// VertexMaterialClass::Apply 0x0013D570 tests the byte in place (`cmp byte [0x00DEC405], 0`); 0x0013C5C0 is the out-of-line copy.
	static bool Is_Coloring_Enabled() { return (IsColoringEnabled == 0) ? false : true; }
	static void Enable_Coloring(unsigned int color);			// 0x00117BF0: when non-zero, overrides the vertex colours
	static bool Is_Snapshot_Activated() { return SnapshotActivated; }
	static void Activate_Snapshot(bool b);						// 0x00116E10 (an out-of-line store in retail)
	static void Set_Thumbnail_Enabled(bool b) { ThumbnailEnabled = b; }
	static bool Get_Thumbnail_Enabled() { return ThumbnailEnabled; }
	static void Set_Screen_UV_Bias(bool onoff) { IsScreenUVBiased = onoff; }
	static bool Is_Screen_UV_Biased(void) { return IsScreenUVBiased; }
	static void Enable_Static_Sort_Lists(bool onoff) { AreStaticSortListsEnabled = onoff; }
	static bool Are_Static_Sort_Lists_Enabled(void) { return AreStaticSortListsEnabled; }
	static void Enable_Munge_Sort_On_Load(bool onoff) { MungeSortOnLoad = onoff; }
	static bool Is_Munge_Sort_On_Load_Enabled(void) { return MungeSortOnLoad; }

private:
	static unsigned int SyncTime;							// 0x009EC3CC
	static unsigned int PreviousSyncTime;					// 0x009EC3D0
	static bool IsInitted;									// 0x009EC3D4
	static bool IsRendering;								// 0x009EC3D5
	static bool IsCapturing;								// 0x009EC3D6
	static bool IsSortingEnabled;							// 0x009B5F7C
	static bool AreDecalsEnabled;							// 0x009B5F7D
	static float DecalRejectionDistance;					// 0x009B5F80
	static bool IsScreenUVBiased;							// 0x009EC3D7
	static bool AreStaticSortListsEnabled;					// 0x009EC3D9
	static bool MungeSortOnLoad;							// 0x009EC3DA
	static FrameGrabClass *Movie;							// 0x009EC3DC
	static bool PauseRecord;								// 0x009EC3C9
	static bool RecordNextFrame;							// 0x009EC3C8
	static int FrameCount;									// 0x009EC3E0
	static StaticSortListClass *DefaultStaticSortLists;		// 0x009EC3F0
	static StaticSortListClass *CurrentStaticSortLists;		// 0x009EC3F4
	static bool SnapshotActivated;							// 0x009EC3FD
	static bool IsCurrentlyRenderingShadowMap;				// 0x009EC404
	static bool IsColoringEnabled;							// 0x009EC405
	static PrelitModeEnum PrelitMode;						// 0x009B5F88
	static bool ThumbnailEnabled;							// 0x009B5F8C
	static NPatchesGapFillingModeEnum NPatchesGapFillingMode;	// 0x009B5F90
	static unsigned NPatchesLevel;							// 0x009B5F94
	static bool IsTexturingEnabled;							// 0x009B5F98
	static int TextureFilter;								// 0x009B5F9C
	static int TextureReductionFactor;						// 0x009EC40C (the data ledger's g_Va00DEC40C)
};

// Overbright shading switch (defined in ww3d.cpp, set from the command line).
extern bool ShaderOverbrightEnabled;

// Texture reduction level (getLODTextureReduction / setLODTextureReduction).
extern int LODTextureReduction;

#endif
