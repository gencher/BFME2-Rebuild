// Portions derived from Command & Conquer Generals and Command & Conquer Generals Zero Hour, (c) Electronic Arts Inc., GPL-3.0-or-later; see LICENSE and NOTICE.
// LineGroupClass.h: WW3D2's renderer for groups of lines (Zero Hour's linegrp.h; BFME 2 keeps its layout).
// Target facts (retail game.dat): constructor 0x001B3490 stores the table 0x00BD7664 at +0, nulls the seven shared
// arrays at +0x4..+0x1C and the line count +0x20 and the texture +0x24, copies the additive sprite preset into the
// shader at +0x28, clears the flags +0x2C and sets the defaults: size +0x30 = 0, colour +0x34..+0x3C = (1, 1, 1),
// alpha +0x40 = 1, U coordinate +0x44 = 0, tail diffuse +0x48..+0x54 = 0, mode +0x58 = 0. SIZE 0x5C. Its accessors
// 0x001B35F0..0x001B3720 read and write those fields; Get_Polygon_Count returns 4 (tetrahedron) or 8 (prism) times
// the line count.
#pragma once
#ifndef BFME2_LINEGROUPCLASS_H
#define BFME2_LINEGROUPCLASS_H

#include "WW3D2/ShaderClass.h"
#include "WW3D2/TextureAsset.h"
#include "WWMath/Vector3.h"
#include "WWMath/Vector4.h"

class RenderInfoClass;
template <class T> class ShareBufferClass;

class LineGroupClass
{
public:
	enum FlagsType
	{
		TRANSFORM	// transform points with the modelview matrix (world-space points)
	};

	enum LineModeType
	{
		TETRAHEDRON,
		PRISM
	};

	LineGroupClass();								// 0x001B3490
	virtual ~LineGroupClass();

	void Set_Line_Size(float size);
	void Set_Arrays(ShareBufferClass<Vector3> *startlocs, ShareBufferClass<Vector3> *endlocs, ShareBufferClass<Vector4> *diffuse = 0, ShareBufferClass<Vector4> *taildiffuse = 0, ShareBufferClass<unsigned int> *alt = 0, ShareBufferClass<float> *sizes = 0, ShareBufferClass<float> *ucoords = 0, int active_line_count = -1);
	float Get_Line_Size();
	void Set_Line_Color(const Vector3 &color);
	Vector3 Get_Line_Color();
	void Set_Tail_Diffuse(const Vector4 &tdiffuse);
	Vector4 Get_Tail_Diffuse();
	void Set_Line_Alpha(float alpha);
	float Get_Line_Alpha();
	void Set_Line_UCoord(float u);
	float Get_Line_UCoord();
	void Set_Flag(FlagsType flag, bool on);
	int Get_Flag(FlagsType flag);
	void Set_Texture(const TextureAsset &texture);
	void Set_Shader(const ShaderClass &shader);
	void Set_Line_Mode(LineModeType linemode);
	int Get_Polygon_Count();
	void Render(RenderInfoClass &rinfo);

	TextureAsset Get_Texture() const;					// shares 0x001790E0 with PointGroupClass
	ShaderClass Get_Shader();

protected:
	ShareBufferClass<Vector3> *StartLineLoc;		// +0x04
	ShareBufferClass<Vector3> *EndLineLoc;			// +0x08
	ShareBufferClass<Vector4> *LineDiffuse;			// +0x0C
	ShareBufferClass<Vector4> *TailDiffuse;			// +0x10
	ShareBufferClass<unsigned int> *ALT;			// +0x14 active line table
	ShareBufferClass<float> *LineSize;				// +0x18
	ShareBufferClass<float> *LineUCoord;			// +0x1C
	int LineCount;									// +0x20
	TextureAsset Texture;							// +0x24
	ShaderClass Shader;								// +0x28
	unsigned int Flags;								// +0x2C
	float DefaultLineSize;							// +0x30
	Vector3 DefaultLineColor;						// +0x34
	float DefaultLineAlpha;							// +0x40
	float DefaultLineUCoord;						// +0x44
	Vector4 DefaultTailDiffuse;						// +0x48
	LineModeType LineMode;							// +0x58
};
// SIZE 0x5C
typedef char LineGroupClassSize[(sizeof(LineGroupClass) == 0x5C) ? 1 : -1];

#endif
