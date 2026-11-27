// Portions derived from Command & Conquer Generals and Command & Conquer Generals Zero Hour, (c) Electronic Arts Inc., GPL-3.0-or-later; see LICENSE and NOTICE.
// StreakRendererClass.h: WW3D2's streak-line renderer (Zero Hour's streakrender.h; its fields follow
// SegLineRendererClass's minus the end width). Target facts (retail game.dat): constructor 0x00742F90 nulls the
// texture +0, copies the additive sprite preset into the shader +4 and sets width +8 = 0, colour +0xC..+0x14 = (1, 1, 1),
// opacity +0x18 = 1, subdivision level +0x1C = 0, noise amplitude +0x20 = 0, merge abort factor +0x24 = 1.5, texture
// tile factor +0x28 = 1, the sync time +0x2C, the UV offset +0x30 and its rate +0x38 = 0, the bits +0x40 = 1 (merge
// intersections) and the vertex buffer size +0x44 and pointer +0x48 = 0. SIZE 0x4C.
#pragma once
#ifndef BFME2_STREAKRENDERERCLASS_H
#define BFME2_STREAKRENDERERCLASS_H

#include "WW3D2/ShaderClass.h"
#include "WW3D2/WW3D.h"
#include "WW3D2/VertexFormats.h"
#include "WW3D2/w3d_file.h"
#include "WWMath/Vector2.h"
#include "WWMath/Vector3.h"

#include "WW3D2/TextureBaseClass.h"
class RenderInfoClass;
class Matrix3D;
class SphereClass;
class Vector4;

class StreakLineClass;

class StreakRendererClass
{
	friend class StreakLineClass;

public:
	enum TextureMapMode
	{
		UNIFORM_WIDTH_TEXTURE_MAP = 0,
		UNIFORM_LENGTH_TEXTURE_MAP,
		TILED_TEXTURE_MAP
	};

	enum
	{
		MERGE_INTERSECTIONS = 0x00000001,
		FREEZE_RANDOM = 0x00000002,
		DISABLE_SORTING = 0x00000004,
		END_CAPS = 0x00000008,
		TEXTURE_MAP_MODE_MASK = 0xFF000000,
		TEXTURE_MAP_MODE_OFFSET = 24
	};

	StreakRendererClass();									// 0x00742F90
	StreakRendererClass(const StreakRendererClass &src);	// 0x00746DE0
	~StreakRendererClass();
	StreakRendererClass &operator=(const StreakRendererClass &that);	// 0x00743100
	void Init(const W3dEmitterLinePropertiesStruct &props);
	void Reset_Line();										// 0x00743060
	void RenderStreak(RenderInfoClass &rinfo, const Matrix3D &transform, unsigned int point_count, Vector3 *points, Vector4 *rgbas, float *widths, const SphereClass &obj_sphere, unsigned int *personalities);

	int Is_Merge_Intersections() const;
	int Is_Freeze_Random() const;
	int Is_Sorting_Disabled() const;
	void Set_Noise_Amplitude(float amplitude);
	void Set_Current_Subdivision_Level(unsigned int level);	// (the level is not stored: retail writes 0)
	void Set_Merge_Intersections(int onoff);
	void Set_Freeze_Random(int onoff);
	void Set_Disable_Sorting(int onoff);
	void Set_End_Caps(int onoff);
	TextureMapMode Get_Texture_Mapping_Mode() const;
	void Set_Current_UV_Offset(const Vector2 &offset);
	void Set_UV_Offset_Rate(const Vector2 &rate);
	void Set_Texture(TextureClass *texture);				// 0x001911E0 (retail's body is shared with SegLineRendererClass::Set_Texture)
	void Set_Shader(ShaderClass shader) { Shader = shader; }
	void Set_Width(float width) { Width = width; }
	void Set_Texture_Tile_Factor(float factor);			// 0x00743056
	void Set_Color(const Vector3 &color) { Color = color; }
	void Set_Opacity(float opacity) { Opacity = opacity; }
	void Set_Texture_Mapping_Mode(TextureMapMode mode)
	{
		Bits &= ~TEXTURE_MAP_MODE_MASK;
		Bits |= (mode << TEXTURE_MAP_MODE_OFFSET);
	}

	friend class StreakLineClass;

private:
	VertexFormatXYZUV1 *getVertexBuffer(unsigned int number);

	TextureClass Texture;				// +0x00 the handle (one factory pointer)
	ShaderClass Shader;					// +0x04
	float Width;						// +0x08
	Vector3 Color;						// +0x0C
	float Opacity;						// +0x18
	unsigned int SubdivisionLevel;		// +0x1C
	float NoiseAmplitude;				// +0x20
	float MergeAbortFactor;				// +0x24
	float TextureTileFactor;			// +0x28
	unsigned int LastUsedSyncTime;		// +0x2C
	Vector2 CurrentUVOffset;			// +0x30
	Vector2 UVOffsetDeltaPerMS;			// +0x38
	unsigned int Bits;					// +0x40
	unsigned int m_vertexBufferSize;	// +0x44
	VertexFormatXYZUV1 *m_vertexBuffer;	// +0x48
};
// SIZE 0x4C
typedef char StreakRendererClassSize[(sizeof(StreakRendererClass) == 0x4C) ? 1 : -1];

#endif
