// Portions derived from Command & Conquer Generals and Command & Conquer Generals Zero Hour, (c) Electronic Arts Inc., GPL-3.0-or-later; see LICENSE and NOTICE.
// MeshLoadContextClass.h: the state MeshModelClass's chunk readers share while one mesh loads (private to
// meshmdlio.cpp in Zero Hour). It owns the pools of shaders, vertex materials and textures the file's material ids
// index, and the legacy material records. The object is destroyed once loading is complete.
//
// Target facts: MeshModelClass::Load_W3D 0x0018BA10 allocates 0x24C bytes and runs the constructor 0x0018B5F0, which
// clears the 0x74-byte header (`rep stos`), the four material counts at +0x78, sets the prelit chunk id +0x88 to -1,
// the current pass +0x8C and texture stage +0x90 to 0, the texture coordinate pointer +0x74 to 0 and the DIG flag
// +0x248 to 0, and builds six DynamicVectorClass members (0x18 bytes each: vftable, array, capacity, two flags,
// count, growth step 10) at +0x94, +0xAC, +0xC4, +0xDC, +0xF4 and +0x10C, the alternate MeshMatDescClass at +0x124
// and a SimpleVecClass of Vector2 at +0x23C. Peek_Shader 0x001886C0 reads the shader array at +0xB0,
// Peek_Vertex_Material 0x001886E0 the array at +0xC8, Peek_Texture 0x001886F0 the array at +0xF8 and the handle
// form 0x00188700 the array at +0x110; Get_Texcoord_Array 0x00188340 allocates NumVertices * 8 bytes into +0x74.
// BFME 2 keeps a second texture vector (+0x10C) of TextureAsset handles beside Zero Hour's TextureClass pointers.
// SIZE 0x24C.
#pragma once
#ifndef BFME2_MESHLOADCONTEXTCLASS_H
#define BFME2_MESHLOADCONTEXTCLASS_H

#include "WWLib/vector.h"
#include "WWLib/simplevec.h"
#include "WWLib/wwstring.h"
#include "WWMath/Vector2.h"
#include "WW3D2/ShaderClass.h"
#include "WW3D2/MeshMatDescClass.h"
#include "WW3D2/TextureAsset.h"
#include "WW3D2/w3d_file.h"

class VertexMaterialClass;
class TextureClass;

class MeshLoadContextClass
{
private:
	MeshLoadContextClass(void);							// 0x0018B5F0
	~MeshLoadContextClass(void);

	W3dTexCoordStruct *Get_Texcoord_Array(void);

	// Zero Hour's counts (public const in the retail mangling; read_per_tri_materials calls them out of line).
public:
	int Shader_Count(void) const;							// 0x00188730
	int Vertex_Material_Count(void) const;						// 0x00188740
	int Texture_Count(void) const;							// 0x00188750
private:

	// The DIG chunk flag (+0x248): the first diffuse-illumination chunk of a mesh goes to the default description, a second one to
	// the alternate. Zero Hour's inline accessors.
	bool Already_Loaded_DIG(void) { return LoadedDIG; }
	void Notify_Loaded_DIG_Chunk(bool onoff) { LoadedDIG = onoff; }

	int Add_Shader(ShaderClass shader);
	int Add_Vertex_Material(VertexMaterialClass *vmat);

	ShaderClass Peek_Shader(int index);
	VertexMaterialClass *Peek_Vertex_Material(int index);
	TextureAsset Peek_Texture_Handle(int index);
	TextureClass *Peek_Texture(int index);

	// Legacy material support.
	ShaderClass Peek_Legacy_Shader(int legacy_material_index);
	Vector2 *Get_Temporary_UV_Array(int elementcount);				// 0x00189400 (grows TempUVArray and returns its data)
	VertexMaterialClass *Peek_Legacy_Vertex_Material(int legacy_material_index);
	TextureAsset Peek_Legacy_Texture(int legacy_material_index);			// 0x001893C0

	struct LegacyMaterialClass
	{
		StringClass Name;
		int VertexMaterialIdx;
		int ShaderIdx;
		int TextureIdx;
	};

	W3dMeshHeader3Struct Header;						// +0x000
	W3dTexCoordStruct *TexCoords;						// +0x074
	W3dMaterialInfoStruct MatInfo;						// +0x078
	unsigned int PrelitChunkID;						// +0x088
	int CurPass;								// +0x08C
	int CurTexStage;							// +0x090
	DynamicVectorClass<LegacyMaterialClass *> LegacyMaterials;		// +0x094
	DynamicVectorClass<ShaderClass> Shaders;				// +0x0AC
	DynamicVectorClass<VertexMaterialClass *> VertexMaterials;		// +0x0C4
	DynamicVectorClass<unsigned long> VertexMaterialCrcs;			// +0x0DC
	DynamicVectorClass<TextureClass *> Textures;				// +0x0F4
	DynamicVectorClass<TextureAsset> TextureHandles;			// +0x10C
	MeshMatDescClass AlternateMatDesc;					// +0x124
	SimpleVecClass<Vector2> TempUVArray;					// +0x23C
	bool LoadedDIG;								// +0x248

	friend class MeshModelClass;
};

typedef char MeshLoadContextClassSize[(sizeof(MeshLoadContextClass) == 0x24C) ? 1 : -1];

#endif
