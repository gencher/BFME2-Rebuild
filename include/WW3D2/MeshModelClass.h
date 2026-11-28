// MeshModelClass.h: WW3D's shared mesh model (geometry plus material
// descriptions and the polygon renderers that draw it).
//
// Target facts:
// - The constructor 0x001727E0 runs MeshGeometryClass's 0x00168F30, installs
//   vftable 0x007D46C4 (6 slots: Delete_This, the deleting destructor
//   0x00172E50, Load_W3D 0x0018BA10, then MeshGeometryClass's three) and the
//   secondary 0x007D46C0, clears +0x8C..+0x98, builds a MultiListClass at
//   +0x9C (vftable 0x007D34E0, head +0xA0), clears +0xB4, +0xB8, +0xBC and the
//   byte +0xC0, links itself at the front of the global model chain (head
//   0x009F6F90: +0xB8 takes the old head, +0xB4 points at the head slot, the
//   old head's +0xB4 points at this +0xB8), sets Flags bit 0, then allocates
//   the default MeshMatDescClass (0x118 bytes, 0x0015B480) into +0x8C and
//   +0x94 and a MaterialInfoClass (0x38 bytes, 0x001708D0) into +0x98.
// - Is_Alternate_Material_Description_Enabled 0x00171830 compares +0x94 with
//   +0x90; Enable_Alternate_Material_Description 0x00171790 switches +0x94
//   between +0x8C and +0x90; the Make_*_Unique forwarders 0x00171760.. call
//   through +0x94.
// - MeshClass::Make_Unique 0x00149C10 allocates 0xC4 bytes for a copy.
// SIZE 0xC4.
#pragma once
#ifndef BFME2_MESHMODELCLASS_H
#define BFME2_MESHMODELCLASS_H

#include "WW3D2/MeshGeometryClass.h"
#include "WWLib/multilist.h"
#include "WW3D2/DX8PolygonRendererClass.h"

#include "WW3D2/MeshMatDescClass.h"

class MaterialInfoClass;
class ChunkLoadClass;
class MeshLoadContextClass;
class FXShaderGeometry;
class VertexMaterialClass;
class DX8FVFCategoryContainer;
class HTreeClass;
class Vector3;

class MeshModelClass : public MeshGeometryClass
{
public:
	MeshModelClass(void);								// 0x001727E0
	MeshModelClass(const MeshModelClass &that);					// 0x00172940
	virtual ~MeshModelClass(void);							// 1: 0x00172AB0

	MeshModelClass &operator=(const MeshModelClass &that);				// 0x00171E60
	void Reset(int polycount, int vertcount, int passcount, bool keep_skin = false);

	void Unregister_For_Rendering(void);							// 0x001716E0

	virtual bool Load_W3D(ChunkLoadClass &cload);					// 2: 0x0018BA10

	void rva00171780(void);								// 0x00171780
	void Enable_Alternate_Material_Description(bool onoff);				// 0x00171790
	bool Is_Alternate_Material_Description_Enabled(void);				// 0x00171830
	bool Needs_Vertex_Normals(void);						// 0x00171850

	// Forwarders through CurMatDesc (+0x94): 0x00171760, 0x00171770.
	void Make_UV_Array_Unique(int pass, int stage);					// 0x00171760
	void Make_Color_Array_Unique(int array);					// 0x00171770
	int Get_Pass_Count(void) const { return CurMatDesc->Get_Pass_Count(); }
	// Forwarders through CurMatDesc (+0x94): W3DBuffBuffer's copy 0x000D204B.
	Vector2 *Get_UV_Array(int index);							// 0x000D204B
	// Forwarders through CurMatDesc (+0x94), expanded by MeshClass::Load_W3D 0x00149CC0.
	bool Has_Shader_Array(int pass = 0) const { return CurMatDesc->Has_Shader_Array(pass); }
	bool Has_Pass_Buffer(int pass = 0) const { return CurMatDesc->Has_Pass_Buffer(pass); }
	ShaderClass Get_Shader(int pidx, int pass = 0) const { return CurMatDesc->Get_Shader(pidx, pass); }
	ShaderClass Get_Single_Shader(int pass = 0) const { return CurMatDesc->Get_Single_Shader(pass); }
	bool Has_Material_Array(int pass = 0) const { return CurMatDesc->Has_Material_Array(pass); }
	unsigned int *Get_Color_Array(int array, bool create = true) { return CurMatDesc->Get_Color_Array(array, create); }
	Vector2 *Get_UV_Array_By_Index(int index, bool create = true) { return CurMatDesc->Get_UV_Array_By_Index(index, create); }
	VertexMaterialClass *Peek_Material(int vidx, int pass = 0) const { return CurMatDesc->Peek_Material(vidx, pass); }
	VertexMaterialClass *Peek_Single_Material(int pass = 0) const { return CurMatDesc->Peek_Single_Material(pass); }
	// Forwarders through CurMatDesc (+0x94), expanded by read_per_tri_materials 0x00189790.
	void Set_Single_Material(VertexMaterialClass *vmat, int pass = 0) { CurMatDesc->Set_Single_Material(vmat, pass); }
	void Set_Single_Texture(const TextureAsset &tex, int pass = 0, int stage = 0) { CurMatDesc->Set_Single_Texture(tex, pass, stage); }
	void Set_Single_Shader(ShaderClass shader, int pass = 0) { CurMatDesc->Set_Single_Shader(shader, pass); }
	void Set_Material(int vidx, VertexMaterialClass *vmat, int pass = 0) { CurMatDesc->Set_Material(vidx, vmat, pass); }
	void Set_Shader(int pidx, ShaderClass shader, int pass = 0) { CurMatDesc->Set_Shader(pidx, shader, pass); }
	void Set_Texture(int pidx, const TextureAsset &tex, int pass = 0, int stage = 0) { CurMatDesc->Set_Texture(pidx, tex, pass, stage); }

	// The FVF container of the first polygon renderer's texture category.
	DX8FVFCategoryContainer *Peek_FVF_Category_Container(void);			// 0x00172030

	void Replace_VertexMaterial(VertexMaterialClass *vmat, VertexMaterialClass *new_vmat);	// 0x00172D40
	void Make_Geometry_Unique(void);						// 0x00172070
	// Skinned positions (and normals) for the htree's current pose.
	void get_deformed_vertices(Vector3 *dst_vert, Vector3 *dst_norm, const HTreeClass *htree);	// 0x0016BF40
	void get_deformed_vertices(Vector3 *dst_vert, const HTreeClass *htree);		// 0x0016B680

protected:
	bool read_vertex_colors(ChunkLoadClass &cload, MeshLoadContextClass *context);		// 0x00188760
	bool read_dcg(ChunkLoadClass &cload, MeshLoadContextClass *context);			// 0x00188BB0
	bool read_dig(ChunkLoadClass &cload, MeshLoadContextClass *context);			// 0x00188DF0
	bool read_shaders(ChunkLoadClass &cload, MeshLoadContextClass *context);		// 0x0018AC90
	bool read_vertex_materials(ChunkLoadClass &cload, MeshLoadContextClass *context);	// 0x0018AD40
	bool read_per_face_texcoord_ids(ChunkLoadClass &cload, MeshLoadContextClass *context);	// 0x00188180
	bool read_vertex_material_ids(ChunkLoadClass &cload, MeshLoadContextClass *context);	// 0x001888F0
	bool read_shader_ids(ChunkLoadClass &cload, MeshLoadContextClass *context);		// 0x001889B0
	bool read_texcoords(ChunkLoadClass &cload, MeshLoadContextClass *context);		// 0x001896D0
	bool read_stage_texcoords(ChunkLoadClass &cload, MeshLoadContextClass *context);	// 0x00189A20
	void post_process(void);								// 0x00189C60
	void install_alternate_material_desc(MeshLoadContextClass *context);		// 0x00188290
	bool read_texture_ids(ChunkLoadClass &cload, MeshLoadContextClass *context);		// 0x00189140
	bool read_textures(ChunkLoadClass &cload, MeshLoadContextClass *context);		// 0x0018AE50
	bool read_texture_stage(ChunkLoadClass &cload, MeshLoadContextClass *context);		// 0x0018B080
	bool read_material_pass(ChunkLoadClass &cload, MeshLoadContextClass *context);		// 0x0018B400
	bool read_prelit_material(ChunkLoadClass &cload, MeshLoadContextClass *context);	// 0x0018B4F0
	bool read_material_info(ChunkLoadClass &cload, MeshLoadContextClass *context);		// inlined at its callers (no retail copy)
	bool read_Rva00188AF0(ChunkLoadClass &cload, MeshLoadContextClass *context);		// 0x00188AF0
	bool read_per_tri_materials(ChunkLoadClass &cload, MeshLoadContextClass *context);	// 0x00189790
	bool read_v3_materials(ChunkLoadClass &cload, MeshLoadContextClass *context);		// 0x0018A750
	bool read_fx_shaders(ChunkLoadClass &cload, MeshLoadContextClass *context);		// 0x0018AF70
	bool read_chunks(ChunkLoadClass &cload, MeshLoadContextClass *context);			// 0x0018B790 (hides MeshGeometryClass::read_chunks)
	void clone_materials(const MeshModelClass &srcmesh);					// 0x001881C0
	void post_process_fog(void);								// 0x00189CD0
	void install_materials(MeshLoadContextClass *context);					// 0x0018A070
	bool read_Rva00189B50(ChunkLoadClass &cload, MeshLoadContextClass *context);		// 0x00189B50
	void Set_Pass_Count(int passes) { CurMatDesc->Set_Pass_Count(passes); }
	unsigned int get_sort_flags(int pass) const;					// 0x00189270
	unsigned int get_sort_flags(void) const;					// 0x001892F0 (every pass's flags ORed)
	void compute_static_sort_levels(void);						// 0x00189320

	// DX8PolygonRendererClass's constructors append to PolygonRendererList.
	friend class DX8PolygonRendererClass;
	friend class DX8MeshRendererClass;
	friend class FXShaderRenderer;
	friend class MeshClass;
	friend MeshModelClass *Get_Next_Mesh_Model(MeshModelClass *current);

	MeshMatDescClass *DefMatDesc;					// +0x8C
	MeshMatDescClass *AlternateMatDesc;				// +0x90
	MeshMatDescClass *CurMatDesc;					// +0x94
	MaterialInfoClass *MatInfo;					// +0x98
	MultiListClass<DX8PolygonRendererClass> PolygonRendererList;	// +0x9C
	void Link_Instance(MeshModelClass **head);
	void Unlink_Instance(void);

	MeshModelClass **ChainPrevLink;					// +0xB4 the slot that points at this model
	MeshModelClass *ChainNext;					// +0xB8 next model in the global chain
	FXShaderGeometry *FXGeometry;					// +0xBC prepared FX render geometry, released by 0x1732E2
	bool Field_C0;							// +0xC0 cleared by the constructor; role not seen yet
};

typedef char MeshModelClassSize[(sizeof(MeshModelClass) == 0xC4) ? 1 : -1];

MeshModelClass *Get_Next_Mesh_Model(MeshModelClass *current);				// 0x001718E0

#endif
