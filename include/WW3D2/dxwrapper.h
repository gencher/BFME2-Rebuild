// Portions derived from Command & Conquer Generals and Command & Conquer Generals Zero Hour, (c) Electronic Arts Inc., GPL-3.0-or-later; see LICENSE and NOTICE.
// dxwrapper.h: WW3D2's Direct3D wrapper (dx8wrapper.cpp, retail object 0x0011CC60 and the header's inlines).
// Class list, member order and inline bodies from Zero Hour's dx8wrapper.h; retail differs in these recorded ways:
// - MAX_TEXTURE_STAGES is 16, not 8: Set_DX8_Texture_Stage_State 0x000661DD guards the stage with `cmp ebx, 0x10`, and
//   RenderStateStruct puts `world` at +0x1EC and `view` at +0x22C (Zero Hour's layout with eight stages has them at
//   +0x1CC and +0x20C: sixteen Textures pointers add 0x20 bytes).
// - The render-state and texture-stage-state setters take the state as `unsigned long` (map names SAXKI, SAXIKI), and
//   both are out of line: 0x0006615F (231 callers) and 0x000661DD.
// - Set_Material 0x000662B1 and Set_Shader 0x000662E5, Clamp_Color 0x00126082 and Convert_Color_Clamp 0x0012617C (its
//   argument by value) are out of line; so is Set_DX8_ZBias. The Convert_Color forms are header inlines (0x0006E1C0 and
//   0x00139290 are their emitted copies).
// - Clear takes a third bool (clear_stencil) before the colour.
// - The Draw_Triangles/Draw_Strip family takes plain `unsigned` parameters (map names SAXIIIII/SAXIIII).
// - The device behind D3DDevice is a Direct3D 9 device (IDirect3DDevice9): retail calls SetTransform (slot 44),
//   SetRenderState (slot 57), SetTextureStageState (slot 67), and SetTexture (slot 65) directly through d3d9.h.
// - Set_Transform(Matrix4x4) stores the projection arm twice: ProjectionMatrix (0x00DEDC30) and then the device copy
//   DeviceProjectionMatrix (0x00DEDBF0), and uploads the second; its world and view arms clear the identity bit
//   before they set the changed bit (retail's Apply rows 0x001844A0, 0x00184FD0, 0x00185E80, 0x00184AD0, 0x00185980).
// Files that include this header need the DirectX opt-in line `// dxsdk` (it includes d3d9.h).
#pragma once
#ifndef BFME2_DXWRAPPER_H
#define BFME2_DXWRAPPER_H

#include <d3d9.h>

#include "WWMath/Matrix3D.h"
#include "WWMath/Matrix4.h"
#include "WWMath/Vector3.h"
#include "WWMath/Vector4.h"
#include "WWMath/wwinline.h"
#include "WW3D2/DX8CleanupHook.h"
#include "WW3D2/ShaderClass.h"
#include "WW3D2/TextureAsset.h"
#include "WW3D2/ww3dformat.h"

// Registry value names of the saved render device (Zero Hour's dx8wrapper.h; retail's literals at 0x00BCFD78..0x00BCFDE8).
#define VALUE_NAME_RENDER_DEVICE_NAME "RenderDeviceName"
#define VALUE_NAME_RENDER_DEVICE_WIDTH "RenderDeviceWidth"
#define VALUE_NAME_RENDER_DEVICE_HEIGHT "RenderDeviceHeight"
#define VALUE_NAME_RENDER_DEVICE_DEPTH "RenderDeviceDepth"
#define VALUE_NAME_RENDER_DEVICE_WINDOWED "RenderDeviceWindowed"
#define VALUE_NAME_RENDER_DEVICE_TEXTURE_DEPTH "RenderDeviceTextureDepth"

const unsigned MAX_TEXTURE_STAGES = 16;
const unsigned MAX_VERTEX_STREAMS = 2;
const unsigned MAX_VERTEX_SHADER_CONSTANTS = 96;
const unsigned MAX_PIXEL_SHADER_CONSTANTS = 8;
const unsigned MAX_SHADOW_MAPS = 1;

enum {
	BUFFER_TYPE_DX8,
	BUFFER_TYPE_SORTING,
	BUFFER_TYPE_DYNAMIC_DX8,
	BUFFER_TYPE_DYNAMIC_SORTING,
	BUFFER_TYPE_INVALID
};

class VertexMaterialClass;
class TextureBaseClass;
class AssetFactoryBase;
class CameraClass;
class LightEnvironmentClass;
class RenderDeviceDescClass;
class RenderInfoClass;
class VertexBufferClass;
class DynamicVBAccessClass;
class IndexBufferClass;
class DynamicIBAccessClass;
class TextureClass;
class ZTextureClass;
class LightClass;
class SurfaceClass;
class StringClass;
class DX8Caps;

// Zero Hour's counters: each DX8 call through the device counts one.
extern unsigned number_of_DX8_calls;		// ?number_of_DX8_calls@@3IA, 0x00DEDA98
extern bool _DX8SingleThreaded;
extern int DX8Wrapper_PreserveFPU;		// set from the command line (parseFPUPreserve)
void DX8_Assert();
void Log_DX8_ErrorCode(unsigned res);		// 0x0011F450 (Zero Hour's DX8_ErrorCode macro tests the HRESULT and calls it)

struct RenderStateStruct
{
	ShaderClass shader;
	VertexMaterialClass *material;
	TextureAsset Textures[MAX_TEXTURE_STAGES];		// +0x08, owning texture handles
	D3DLIGHT9 Lights[4];
	bool LightEnable[4];
	Matrix4x4 world;		// +0x1EC
	Matrix4x4 view;		// +0x22C
	unsigned vertex_buffer_types[MAX_VERTEX_STREAMS];
	unsigned index_buffer_type;
	unsigned short vba_offset;
	unsigned short vba_count;
	unsigned short iba_offset;
	VertexBufferClass *vertex_buffers[MAX_VERTEX_STREAMS];
	IndexBufferClass *index_buffer;
	unsigned short index_base_offset;

	RenderStateStruct();
	~RenderStateStruct();

	RenderStateStruct &operator=(const RenderStateStruct &src);
};

class DXWrapper
{
	enum ChangedStates {
		WORLD_CHANGED = 1 << 0,
		VIEW_CHANGED = 1 << 1,
		LIGHT0_CHANGED = 1 << 2,
		LIGHT1_CHANGED = 1 << 3,
		LIGHT2_CHANGED = 1 << 4,
		LIGHT3_CHANGED = 1 << 5,
		TEXTURE0_CHANGED = 1 << 6,
		TEXTURE1_CHANGED = 1 << 7,
		TEXTURE2_CHANGED = 1 << 8,
		TEXTURE3_CHANGED = 1 << 9,
		MATERIAL_CHANGED = 1 << 14,
		SHADER_CHANGED = 1 << 15,
		VERTEX_BUFFER_CHANGED = 1 << 16,
		INDEX_BUFFER_CHANGED = 1 << 17,
		WORLD_IDENTITY = 1 << 18,
		VIEW_IDENTITY = 1 << 19,

		TEXTURES_CHANGED = TEXTURE0_CHANGED | TEXTURE1_CHANGED | TEXTURE2_CHANGED | TEXTURE3_CHANGED,
		LIGHTS_CHANGED = LIGHT0_CHANGED | LIGHT1_CHANGED | LIGHT2_CHANGED | LIGHT3_CHANGED,
	};

	static void Draw_Sorting_IB_VB(unsigned primitive_type, unsigned start_index, unsigned polygon_count, unsigned min_vertex_index, unsigned vertex_count);	// 0x0011D710
	static void Get_Render_Target_Resolution(int &set_w, int &set_h, int &set_bits, bool &set_windowed);	// 0x00121860
	static void Draw(unsigned primitive_type, unsigned start_index, unsigned polygon_count, unsigned min_vertex_index, unsigned vertex_count, bool wait_for_back_buffer = false);	// 0x001203C0

public:
	static bool Init(void *hwnd, bool lite = false);										// 0x00125620
	static void Shutdown(void);																// 0x00125DC0
	static void Do_Onetime_Device_Dependent_Inits(void);									// 0x001245A0
	static void Do_Onetime_Device_Dependent_Shutdowns(void);								// 0x001215F0

	static bool Is_Device_Lost() { return IsDeviceLost; }
	static bool Is_Initted(void) { return IsInitted; }

	static bool Has_Stencil(void);															// 0x0011CDC0
	static void Get_Format_Name(unsigned int format, StringClass *tex_format);				// 0x0011FE20

	static void Begin_Scene_Inner(void);													// 0x0011D2B0
	static void Wait_For_Back_Buffer(void);												// 0x0011D2D0 (locks and unlocks the back buffer: a frame sync; no Zero Hour counterpart)
	static void End_Scene(bool flip_frame = true);											// 0x00122BE0
	static void Flip_To_Primary(void);														// 0x00122DF0

	static void Clear(bool clear_color, bool clear_z_stencil, bool clear_stencil, const Vector3 &color, float dest_alpha = 0.0f, float z = 1.0f, unsigned int stencil = 0);	// 0x0011D330

	static void Set_Viewport(CONST D3DVIEWPORT9 *pViewport);								// 0x0011D480

	static void Set_Vertex_Buffer(const VertexBufferClass *vb, unsigned stream = 0);		// 0x0011D4A0
	static void Set_Vertex_Buffer(const DynamicVBAccessClass &vba);							// 0x0011D5C0
	static void Set_Index_Buffer(const IndexBufferClass *ib, unsigned short index_base_offset);	// 0x0011D530
	static void Set_Index_Buffer(const DynamicIBAccessClass &iba, unsigned short index_base_offset);	// 0x0011D690
	static void Set_Index_Buffer_Index_Offset(unsigned offset);								// 0x00083B81

	static void Get_Render_State(RenderStateStruct &state);									// 0x0012E010
	static void Set_Render_State(const RenderStateStruct &state);							// 0x0012E3E0
	static void Release_Render_State();														// 0x0011C500

	static void Set_DX8_Material(const D3DMATERIAL9 *mat);									// 0x0013C930

	static void Set_Gamma(float gamma, float bright, float contrast, bool calibrate = true, bool uselimit = true);	// 0x00120980

	static void Set_DX8_ZBias(int zbias);													// out of line in retail (0x00905990 in BFME 1)
	static void Set_Projection_Transform_With_Z_Bias(const Matrix4x4 &matrix, float znear, float zfar);	// 0x00120CD0

	static void Set_Transform(D3DTRANSFORMSTATETYPE transform, const Matrix4x4 &m);			// inline here; out-of-line copy 0x000A68A7
	static void Set_Transform(D3DTRANSFORMSTATETYPE transform, const Matrix3D &m);			// out-of-line copy 0x0006A553
	static void Get_Transform(D3DTRANSFORMSTATETYPE transform, Matrix4x4 &m);					// 0x000768FE
	static void Set_World_Identity();														// 0x0010737E
	static void Set_View_Identity();														// 0x001100E8

	static void Set_DX8_Light(int index, D3DLIGHT9 *light);									// 0x0011C470
	static void Set_DX8_Render_State(unsigned long state, unsigned value);					// 0x0006615F
	static void Set_DX8_Texture_Stage_State(unsigned stage, unsigned long state, unsigned value);	// 0x000661DD
	static void Set_DX8_Texture(unsigned int stage, IDirect3DBaseTexture9 *texture);		// 0x00075412
	static void Set_Light_Environment(LightEnvironmentClass *light_env);					// 0x00122EA0

	static void Set_Shader(const ShaderClass &shader);										// 0x000662E5
	static void Get_Shader(ShaderClass &shader);											// 0x0010852B
	static void Set_Material(const VertexMaterialClass *material);							// 0x000662B1
	static void Set_Light(unsigned index, const D3DLIGHT9 *light);							// 0x0011DD20
	static void Set_Light(unsigned index, const LightClass &light);							// 0x0011DD80

	static void Apply_Render_State_Changes();												// 0x0011D930

	static void Draw_Triangles(unsigned buffer_type, unsigned start_index, unsigned polygon_count, unsigned min_vertex_index, unsigned vertex_count);	// 0x001205B0
	static void Draw_Triangles(unsigned start_index, unsigned polygon_count, unsigned min_vertex_index, unsigned vertex_count);	// 0x00120620
	static void Draw_Strip(unsigned start_index, unsigned index_count, unsigned min_vertex_index, unsigned vertex_count);	// 0x001206D0

	// Non-indexed primitive wrappers, one shape (retail 0x00120650..0x00120700, 29 bytes each): Draw(type, 0, n, start, vertices, false)
	// with D3DPT_TRIANGLELIST/LINELIST/LINESTRIP/POINTLIST/TRIANGLESTRIP and 3n, 2n, n+1, n, n+2 vertices. The map names the
	// first and last bfmeRva...; the three between are placeholders that call the private-in-Zero-Hour Draw, so they are members.
	static void bfmeRva00120650(unsigned start_vertex, unsigned polygon_count);			// 0x00120650
	static void Draw_Line_List(unsigned start_vertex, unsigned line_count);					// 0x00120670
	static void Draw_Line_Strip(unsigned start_vertex, unsigned line_count);				// 0x00120690
	static void Draw_Point_List(unsigned start_vertex, unsigned point_count);				// 0x001206B0
	static void bfmeRva00120700(unsigned start_vertex, unsigned polygon_count);			// 0x00120700

	static IDirect3DSurface9 *_Create_DX8_Surface(unsigned int width, unsigned int height, WW3DFormat format, D3DPOOL pool);	// 0x0011DC60
	static IDirect3DSurface9 *_Get_DX8_Front_Buffer();										// 0x0011DFE0

	static unsigned int Get_Free_Texture_RAM();												// 0x0011E630
	static void AdjustTextureSize(unsigned &width, unsigned &height);						// 0x0011E670

	static D3DFORMAT Get_D3D_Back_Buffer_Format(void);	// 0x0011F1B0
	static int Get_Render_Device_Count(void);		// 0x00120130

	static unsigned long Get_FrameCount(void) { return FrameCount; }
	static int Get_Texture_Bit_Depth(void) { return TextureBitDepth; }
	static const D3DADAPTER_IDENTIFIER9 &Get_Current_Adapter_Identifier() { return CurrentAdapterIdentifier; }

	static void Begin_Statistics();															// 0x0011D140
	static void End_Statistics();															// 0x0011D180

	// Out-of-line in retail, one six-byte getter each (0x0011D200..0x0011D290, in this order); Zero Hour's names.
	static unsigned Get_Last_Frame_Matrix_Changes();										// 0x0011D200
	static unsigned Get_Last_Frame_Material_Changes();										// 0x0011D210
	static unsigned Get_Last_Frame_Vertex_Buffer_Changes();									// 0x0011D220
	static unsigned Get_Last_Frame_Index_Buffer_Changes();									// 0x0011D230
	static unsigned Get_Last_Frame_Light_Changes();											// 0x0011D240
	static unsigned Get_Last_Frame_Texture_Changes();										// 0x0011D250
	static unsigned Get_Last_Frame_Render_State_Changes();									// 0x0011D260
	static unsigned Get_Last_Frame_Texture_Stage_State_Changes();							// 0x0011D270
	static unsigned Get_Last_Frame_Number_Of_DX8_Calls();									// 0x0011D280
	static unsigned Get_Last_Frame_Draw_Calls();											// 0x0011D290

	static void Set_Fog(bool enable, const Vector3 &color, float start, float end);
	static bool Get_Fog_Enable() { return FogEnable; }
	static D3DCOLOR Get_Fog_Color() { return FogColor; }

	// The two Convert_Color(Vector3/Vector4) forms are header inlines below (the Vector3 one holds Zero Hour's asm block; out-of-line copies 0x0006E1C0 and 0x00139290); Clamp_Color is out of line.
	static Vector4 Convert_Color(unsigned color);											// 0x000EDF46
	static unsigned int Convert_Color(const Vector4 &color);								// 0x00139290
	static unsigned int Convert_Color(const Vector3 &color, const float alpha);				// 0x0006E1C0
	static void Clamp_Color(Vector4 &color);												// 0x00126082
	static unsigned int Convert_Color_Clamp(Vector4 color);									// 0x0012617C

	static void _Enable_Triangle_Draw(bool enable);											// 0x0012D2C0 (out of line in retail)
	static bool _Is_Triangle_Draw_Enabled() { return _EnableTriangleDraw; }

	static IDirect3DSwapChain9 *Create_Additional_Swap_Chain(void *render_window);			// 0x0011E580
	static void Set_Render_Target(IDirect3DSurface9 *render_target, bool use_default_depth_buffer = false);	// 0x0011E250
	static void Set_Render_Target(IDirect3DSurface9 *render_target, IDirect3DSurface9 *depth_buffer);	// 0x0011E3E0
	static void Set_Render_Target(IDirect3DSwapChain9 *swap_chain);							// 0x00120940
	static bool Is_Render_To_Texture(void) { return IsRenderToTexture; }
	static void Set_Render_Target_With_Z(TextureClass *texture, ZTextureClass *ztexture = 0);	// 0x00120850

	static void Apply_Default_State();														// 0x00123170

	static void Set_Vertex_Shader_Constant(int reg, const void *data, int count);			// 0x00075343
	static void Set_Pixel_Shader_Constant(int reg, const void *data, int count);			// 0x00075399

	static void Set_Ambient(const Vector3 &color);											// 0x00117F50
	static const Vector3 &Get_Ambient() { return Ambient_Color; }

	static IDirect3DDevice9 *_Get_D3D_Device8() { return D3DDevice; }
	static IDirect3D9 *_Get_D3D8() { return D3DInterface; }

	static bool Reset_Device(bool reload_assets = true);									// 0x00122600
	static const DX8Caps *Get_Current_Caps() { return CurrentCaps; }

	static bool Registry_Save_Render_Device(const char *sub_key);							// 0x00121930
	static bool Registry_Load_Render_Device(const char *sub_key, bool resize_window);		// 0x00125A80

	static const char *Get_DX8_Render_State_Name(unsigned long state);						// 0x0011E6D0
	static const char *Get_DX8_Texture_Stage_State_Name(unsigned long state);				// 0x0011EA70
	static unsigned Get_DX8_Render_State(unsigned long state) { return RenderStates[state]; }

	static void Get_DX8_Render_State_Value_Name(StringClass &name, unsigned long state, unsigned value);	// 0x00121A10
	static void Get_DX8_Texture_Stage_State_Value_Name(StringClass &name, unsigned long state, unsigned value);	// 0x00120B90
	static const char *Get_DX8_Texture_Op_Name(unsigned value);								// 0x0011EB60
	static const char *Get_DX8_Texture_Arg_Name(unsigned value);							// 0x0011EC90
	static const char *Get_DX8_Texture_Filter_Name(unsigned value);							// 0x0011ED30
	static const char *Get_DX8_Texture_Address_Name(unsigned value);						// 0x0011ED70
	static const char *Get_DX8_Texture_Transform_Flag_Name(unsigned value);					// 0x0011EDC0
	static const char *Get_DX8_ZBuffer_Type_Name(unsigned value);							// 0x0011EE20
	static const char *Get_DX8_Fill_Mode_Name(unsigned value);								// 0x0011EE50
	static const char *Get_DX8_Shade_Mode_Name(unsigned value);								// 0x0011EE80
	static const char *Get_DX8_Blend_Name(unsigned value);									// 0x0011EEB0
	static const char *Get_DX8_Cull_Mode_Name(unsigned value);								// 0x0011EF50
	static const char *Get_DX8_Cmp_Func_Name(unsigned value);								// 0x0011EF80
	static const char *Get_DX8_Fog_Mode_Name(unsigned value);								// 0x0011EFF0
	static const char *Get_DX8_Stencil_Op_Name(unsigned value);								// 0x0011F030
	static const char *Get_DX8_Material_Source_Name(unsigned value);						// 0x0011F0A0
	static const char *Get_DX8_Vertex_Blend_Flag_Name(unsigned value);						// 0x0011F0D0
	static const char *Get_DX8_Patch_Edge_Style_Name(unsigned value);						// 0x0011F120
	static const char *Get_DX8_Debug_Monitor_Token_Name(unsigned value);					// 0x0011F140
	static const char *Get_DX8_Blend_Op_Name(unsigned value);								// 0x0011F160

	static void Invalidate_Cached_Render_States(void);										// 0x0011FD10
	static void Invalidate_Texture_Stages(void);											// 0x0011CC60 (the texture-stage half of the invalidation; the map's placeholder row)
	static void Set_Skip_Fixed_Function_State(bool skip);									// 0x00125430 (role name: the FX shader pass turns the fixed-function light state off, and on again by re-applying the light environment)

protected:
	static bool Create_Device(void);														// 0x001246C0
	static void Release_Device(void);														// 0x00121770

	static void Reset_Statistics();															// 0x0011D0D0
	static void Enumerate_Devices();														// 0x00124890
	static void Set_Default_Global_Render_States(void);										// 0x00122510

	static bool Set_Any_Render_Device(void);												// 0x00125900
	static bool Set_Render_Device(const char *dev_name, int width = -1, int height = -1, int bits = -1, int windowed = -1, bool resize_window = false);	// 0x00125970
	static bool Set_Render_Device(int dev = -1, int resx = -1, int resy = -1, int bits = -1, int windowed = -1, bool resize_window = false, bool reset_device = false, bool restore_assets = true);	// 0x00124FF0
	static bool Set_Next_Render_Device(void);												// 0x00125400

	static int Get_Render_Device(void);														// 0x0011CDE0
	static const RenderDeviceDescClass &Get_Render_Device_Desc(int deviceidx);				// 0x00120140
	static const char *Get_Render_Device_Name(int device_index);							// 0x00120180
	static bool Set_Device_Resolution(int width = -1, int height = -1, int bits = -1, int windowed = -1, bool resize_window = false);	// 0x00122AD0
	static void Get_Device_Resolution(int &set_w, int &set_h, int &set_bits, bool &set_windowed);	// 0x0011CDF0
	static int Get_Device_Resolution_Width(void) { return ResolutionWidth; }
	static int Get_Device_Resolution_Height(void) { return ResolutionHeight; }

	static bool Registry_Save_Render_Device(const char *sub_key, int device, int width, int height, int depth, bool windowed, int texture_depth);	// 0x001201A0
	static bool Registry_Load_Render_Device(const char *sub_key, char *device, int device_len, int &width, int &height, int &depth, int &windowed, int &texture_depth);	// 0x0011CE40
	static bool Is_Windowed(void) { return IsWindowed; }

	static void Set_Swap_Interval(int swap);												// 0x00122A60

	static bool Find_Color_And_Z_Mode(int resx, int resy, int bitdepth, D3DFORMAT *set_colorbuffer, D3DFORMAT *set_backbuffer, D3DFORMAT *set_zmode);	// 0x00121970
	static bool Find_Color_Mode(D3DFORMAT colorbuffer, int resx, int resy, UINT *mode);		// 0x0011CF80
	static bool Find_Z_Mode(D3DFORMAT colorbuffer, D3DFORMAT backbuffer, D3DFORMAT *zmode);	// 0x001202B0
	static bool Test_Z_Mode(D3DFORMAT colorbuffer, D3DFORMAT backbuffer, D3DFORMAT zmode);	// 0x0011D080

	// Protected member variables, Zero Hour's order. Addresses are the data identities' (old-ledger names, unproven).
	static DX8_CleanupHook *m_pCleanupHook;

	static RenderStateStruct render_state;													// 0x00DEE5D8
	static unsigned render_state_changed;													// 0x00DEC4F4

	static bool IsInitted;																	// 0x00DEDA04
	static bool IsDeviceLost;																// 0x00DEC4F8
	static void *Hwnd;
	static unsigned _MainThreadID;

	static bool _EnableTriangleDraw;														// 0x009B5FCD (.data)

	static int CurRenderDevice;																// 0x009B5FD0 (.data)
	static int ResolutionWidth;
	static int ResolutionHeight;
	static int BitDepth;
	static int TextureBitDepth;
	static bool IsWindowed;																	// 0x00DEDA06
	static D3DFORMAT DisplayFormat;															// 0x00DEDA08

	static D3DMATRIX old_world;																// 0x00DEC500
	static D3DMATRIX old_view;																// 0x00DEC5A0
	static D3DMATRIX old_prj;																// 0x00DEC558

	static DWORD Vertex_Shader;
	static DWORD Pixel_Shader;

	static Vector4 Vertex_Shader_Constants[MAX_VERTEX_SHADER_CONSTANTS];
	static Vector4 Pixel_Shader_Constants[MAX_PIXEL_SHADER_CONSTANTS];

	static LightEnvironmentClass *Light_Environment;										// 0x00DEDA14
	static bool SkipFixedFunctionState;														// 0x00DEDA20 (tested by Set_Light_Environment and Apply_Render_State_Changes)
	static RenderInfoClass *Render_Info;

	static DWORD Vertex_Processing_Behavior;												// 0x00DEDA1C

	static ZTextureClass *Shadow_Map[MAX_SHADOW_MAPS];

	static Vector3 Ambient_Color;

	static bool world_identity;
	static unsigned RenderStates[256];														// 0x00DED5F8
	static unsigned TextureStageStates[MAX_TEXTURE_STAGES][32];								// 0x00DECA38
	static IDirect3DBaseTexture9 *Textures[MAX_TEXTURE_STAGES];								// 0x00DEC4B0

	static bool FogEnable;																	// 0x00DEDA21
	static D3DCOLOR FogColor;																// 0x00DEDA24
	static float FogStart;	// RVA 0x009EDA28
	static float FogEnd;	// RVA 0x009EDA2C

	static unsigned matrix_changes;															// 0x00DEDA4C
	static unsigned material_changes;														// 0x00DEDA50
	static unsigned vertex_buffer_changes;													// 0x00DEDA54
	static unsigned index_buffer_changes;													// 0x00DEDA58
	static unsigned light_changes;
	static unsigned texture_changes;
	static unsigned render_state_changes;													// 0x00DEDA64
	static unsigned texture_stage_state_changes;
	static unsigned draw_calls;
	// The previous frame's counters (End_Statistics copies the live ones here), zero-initialised statics at
	// 0x00DEDA9C..0x00DEDAC0 (ten dwords, the data ledger's G009EDA9C.. names); order from Zero Hour's file statics.
	static unsigned last_frame_matrix_changes;
	static unsigned last_frame_material_changes;
	static unsigned last_frame_vertex_buffer_changes;
	static unsigned last_frame_index_buffer_changes;
	static unsigned last_frame_light_changes;
	static unsigned last_frame_texture_changes;
	static unsigned last_frame_render_state_changes;
	static unsigned last_frame_texture_stage_state_changes;
	static unsigned last_frame_number_of_DX8_calls;
	static unsigned last_frame_draw_calls;
	static bool CurrentDX8LightEnables[4];													// 0x00DED5F0

	static unsigned long FrameCount;

	static DX8Caps *CurrentCaps;

	static D3DADAPTER_IDENTIFIER9 CurrentAdapterIdentifier;									// 0x00DEC5E8

	static IDirect3D9 *D3DInterface;														// 0x00DEDA30
	static IDirect3DDevice9 *D3DDevice;														// 0x00DEDA34

	static IDirect3DSurface9 *CurrentRenderTarget;											// 0x00DEDA38
	static IDirect3DSurface9 *CurrentDepthBuffer;											// 0x00DEDA3C
	static IDirect3DSurface9 *DefaultRenderTarget;											// 0x00DEDA40
	static IDirect3DSurface9 *DefaultDepthBuffer;											// 0x00DEDA44

	static unsigned DrawPolygonLowBoundLimit;

	static bool IsRenderToTexture;															// 0x00DEDA48

	static float ZBias;													// 0x00DEDA74 (the float depth bias bfmeSetProjectionDepthBias stores)
	static float ZNear;																		// 0x00DEC4F0
	static float ZFar;																		// 0x00DED9F8
	static Matrix4x4 ProjectionMatrix;														// 0x00DEDC30
	static Matrix4x4 DeviceProjectionMatrix;													// 0x00DEDBF0 (retail's uploaded copy)

	friend void DX8_Assert();
	friend void bfmeSetProjectionDepthBias(float bias);
	friend class WW3D;
	friend bool SaveRenderDeviceRegistrySettings(const char *sub_key, int device, int width, int height, int depth, bool windowed, int texture_depth);
	static void Set_Texture(unsigned int stage, IDirect3DBaseTexture9 *texture);

	friend class TextureFilterClass;
	friend class TextureAsset::Impl;
	friend class DX8IndexBufferClass;
	friend class DX8VertexBufferClass;
};

// SetTexture is slot 65 (+0x104).
WWINLINE void DXWrapper::Set_Texture(unsigned int stage, IDirect3DBaseTexture9 *texture)
{
	if (stage >= MAX_TEXTURE_STAGES) {
		D3DDevice->SetTexture(stage, texture);
		number_of_DX8_calls++;
		return;
	}

	if (Textures[stage] == texture) return;

	if (Textures[stage]) Textures[stage]->Release();
	Textures[stage] = texture;
	if (Textures[stage]) Textures[stage]->AddRef();
	D3DDevice->SetTexture(stage, texture);
	number_of_DX8_calls++;
	texture_changes++;
}

// evidence: retail 0x001844A0, 0x00184FD0, 0x00185E80, 0x00184AD0 and 0x00185980 (the mapper Apply rows) carry this
// body expanded in place (1,266 to 1,288 bytes each, no call to DXWrapper::Set_Transform); the out-of-line copy is
// 0x000A68A7. The device call goes through slot 44 (+0xB0), then number_of_DX8_calls counts it.
#define DX8_SET_TRANSFORM(device, type, matrix) (device)->SetTransform((type), (const D3DMATRIX *)(matrix))

WWINLINE void DXWrapper::Set_Transform(D3DTRANSFORMSTATETYPE transform, const Matrix4x4 &m)
{
	switch ((int)transform) {
	case D3DTS_WORLD:
		render_state.world = m.Transpose();
		render_state_changed = (render_state_changed & ~(unsigned)WORLD_IDENTITY) | (unsigned)WORLD_CHANGED;
		break;
	case D3DTS_VIEW:
		render_state.view = m.Transpose();
		render_state_changed = (render_state_changed & ~(unsigned)VIEW_IDENTITY) | (unsigned)VIEW_CHANGED;
		break;
	case D3DTS_PROJECTION:
		DeviceProjectionMatrix = ProjectionMatrix = m.Transpose();
		ZFar = 0.0f;
		ZNear = 0.0f;
		DX8_SET_TRANSFORM(_Get_D3D_Device8(), D3DTS_PROJECTION, &DeviceProjectionMatrix);
		number_of_DX8_calls++;
		break;
	default:
		matrix_changes++;
		Matrix4x4 transposed = m.Transpose();
		DX8_SET_TRANSFORM(_Get_D3D_Device8(), transform, &transposed);
		number_of_DX8_calls++;
		break;
	}
}

// evidence: retail 0x0013D570 (VertexMaterialClass::Apply) and 0x0013D180 expand it in place: material_changes++ at 0x00DEDA50, then
// the device's SetMaterial (slot 49, `call [edx+0xC4]`), then number_of_DX8_calls++; 0x0013C930 is the out-of-line copy.
WWINLINE void DXWrapper::Set_DX8_Material(const D3DMATERIAL9 *mat)
{
	material_changes++;
	_Get_D3D_Device8()->SetMaterial(mat);
	number_of_DX8_calls++;
}

// SceneClass::Render expands this setter with the same shader invalidation
// as the Westwood implementation. BFME 2 also caches the two fog depths.
WWINLINE void DXWrapper::Set_Fog(bool enable, const Vector3 &color, float start, float end)
{
	FogEnable = enable;
	FogColor = Convert_Color(color, 0.0f);
	FogStart = start;
	FogEnd = end;
	ShaderClass::Invalidate();
	Set_DX8_Render_State(D3DRS_FOGSTART, *(DWORD *)(&start));
	Set_DX8_Render_State(D3DRS_FOGEND, *(DWORD *)(&end));
}

// Decode the packed colour in place in the mesh loaders. Their colour-modulation
// loops use the components before packing them back into D3DCOLOR.
WWINLINE Vector4 DXWrapper::Convert_Color(unsigned color)
{
	Vector4 col;
	col[3]=((color&0xff000000)>>24)/255.0f;
	col[0]=((color&0xff0000)>>16)/255.0f;
	col[1]=((color&0xff00)>>8)/255.0f;
	col[2]=((color&0xff)>>0)/255.0f;
	return col;
}

// Convert an RGBA colour from floats in [0, 1] to a 32-bit D3DCOLOR (Zero Hour's block, WWINLINE): the four products are
// stored with fistp under the truncate rounding mode, which the block switches on and restores. Retail expands it in
// place in the mesh colour readers and other renderer paths. The out-of-line copy uses the same rounding mode.
WWINLINE unsigned int DXWrapper::Convert_Color(const Vector3 &color, float alpha)
{
	const float scale = 255.0;
	unsigned int col = 0;

	// asm-evidence: DXWrapper::Convert_Color(const Vector3 &, float) 0x0006E1C0 fistp: the fstcw/fldcw rounding-mode switch around four fistp has no C++ form; retail 0x0006E1C0 is this block
	__asm
	{
		sub	esp,20					// space for a, r, g and b float plus fpu rounding mode


		fwait
		fstcw		[esp+16]				// store control word to stack
		mov		eax,[esp+16]		// load it to eax
		mov		edi,eax				// take copy
		and		eax,~(1024|2048)	// mask out certain bits
		or			eax,(1024|2048)	// or with precision control value "truncate"
		sub		edi,eax				// did it change?
		jz			skip					// .. if not, skip
		mov		[esp],eax			// .. change control word
		fldcw		[esp]
skip:


		mov	esi,dword ptr color
		fld	dword ptr[scale]

		fld	dword ptr[esi]			// r
		fld	dword ptr[esi+4]		// g
		fld	dword ptr[esi+8]		// b
		fld	dword ptr[alpha]		// a
		fld	st(4)
		fmul	st(4),st
		fmul	st(3),st
		fmul	st(2),st
		fmulp	st(1),st
		fistp	dword ptr[esp+0]		// a
		fistp	dword ptr[esp+4]		// b
		fistp	dword ptr[esp+8]		// g
		fistp	dword ptr[esp+12]		// r
		mov	ecx,[esp]				// a
		mov	eax,[esp+4]				// b
		mov	edx,[esp+8]				// g
		mov	ebx,[esp+12]			// r
		shl	ecx,24					// a << 24
		shl	ebx,16					// r << 16
		shl	edx,8						//	g << 8
		or		eax,ecx					// (a << 24) | b
		or		eax,ebx					// (a << 24) | (r << 16) | b
		or		eax,edx					// (a << 24) | (r << 16) | (g << 8) | b

		fstp	st(0)


		cmp	edi,0					// did we change the value?
		je		not_changed			// nope... skip now...
		fwait
		fldcw	[esp+16];
not_changed:
		add	esp,20

		mov	col,eax
	}
	return col;
}

// The Vector4 form forwards to it (Zero Hour); retail's 0x00139290 holds the expansion.
WWINLINE unsigned int DXWrapper::Convert_Color(const Vector4 &color)
{
	return Convert_Color(reinterpret_cast<const Vector3 &>(color), color[3]);
}

#endif
