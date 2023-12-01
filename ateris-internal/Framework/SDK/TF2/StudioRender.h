#pragma once
#include "Cvar.h"
#include "Vector.h"
#include "MaterialSystem.h"

struct studiohdr_t;
struct studiomeshdata_t;
struct LightDesc_t;
class IMaterial;
struct studiohwdata_t;
struct Ray_t;
class IMaterialSystem;
struct vertexFileHeader_t;
struct FlashlightState_t;
struct FileHeader_t;
class IPooledVBAllocator;
typedef unsigned short MDLHandle_t;

typedef void ( *StudioRender_Printf_t )( const char* fmt, ... );

struct StudioRenderConfig_t
{
	float fEyeShiftX;
	float fEyeShiftY;
	float fEyeShiftZ;
	float fEyeSize;
	float fEyeGlintPixelWidthLODThreshold;
	int maxDecalsPerModel;
	int drawEntities;
	int skin;
	int fullbright;
	bool bEyeMove : 1;
	bool bSoftwareSkin : 1;
	bool bNoHardware : 1;
	bool bNoSoftware : 1;
	bool bTeeth : 1;
	bool bEyes : 1;
	bool bFlex : 1;
	bool bWireframe : 1;
	bool bDrawNormals : 1;
	bool bDrawTangentFrame : 1;
	bool bDrawZBufferedWireframe : 1;
	bool bSoftwareLighting : 1;
	bool bShowEnvCubemapOnly : 1;
	bool bWireframeDecals : 1;
	int m_nReserved[ 4 ];
};

using StudioDecalHandle_t = void*;
#define STUDIORENDER_DECAL_INVALID  ( (StudioDecalHandle_t)0 )

enum
{
	STUDIORENDER_DRAW_ENTIRE_MODEL = 0,
	STUDIORENDER_DRAW_OPAQUE_ONLY = 0x01,
	STUDIORENDER_DRAW_TRANSLUCENT_ONLY = 0x02,
	STUDIORENDER_DRAW_GROUP_MASK = 0x03,
	STUDIORENDER_DRAW_NO_FLEXES = 0x04,
	STUDIORENDER_DRAW_STATIC_LIGHTING = 0x08,
	STUDIORENDER_DRAW_ACCURATETIME = 0x10,
	STUDIORENDER_DRAW_NO_SHADOWS = 0x20,
	STUDIORENDER_DRAW_GET_PERF_STATS = 0x40,
	STUDIORENDER_DRAW_WIREFRAME = 0x80,
	STUDIORENDER_DRAW_ITEM_BLINK = 0x100,
	STUDIORENDER_SHADOWDEPTHTEXTURE = 0x200,
	STUDIORENDER_SSAODEPTHTEXTURE = 0x1000,
	STUDIORENDER_GENERATE_STATS = 0x8000
};

#define	VERTEX_POSITION 0x0001
#define	VERTEX_NORMAL 0x0002
#define	VERTEX_COLOR 0x0004
#define	VERTEX_SPECULAR 0x0008
#define	VERTEX_TANGENT_S 0x0010
#define	VERTEX_TANGENT_T 0x0020
#define	VERTEX_TANGENT_SPACE ( VERTEX_TANGENT_S | VERTEX_TANGENT_T )
#define	VERTEX_WRINKLE 0x0040
#define	VERTEX_BONE_INDEX 0x0080
#define	VERTEX_FORMAT_VERTEX_SHADER 0x0100
#define	VERTEX_FORMAT_USE_EXACT_FORMAT 0x0200
#define	VERTEX_FORMAT_COMPRESSED 0x400
#define	VERTEX_LAST_BIT 10
#define	VERTEX_BONE_WEIGHT_BIT (VERTEX_LAST_BIT + 1)
#define	USER_DATA_SIZE_BIT (VERTEX_LAST_BIT + 4)
#define	TEX_COORD_SIZE_BIT (VERTEX_LAST_BIT + 7)
#define	VERTEX_BONE_WEIGHT_MASK ( 0x7 << VERTEX_BONE_WEIGHT_BIT )
#define	USER_DATA_SIZE_MASK ( 0x7 << USER_DATA_SIZE_BIT )
#define	VERTEX_FORMAT_FIELD_MASK 0x0FF
#define	VERTEX_FORMAT_UNKNOWN 0
#define VERTEX_BONEWEIGHT( _n ) ((_n) << VERTEX_BONE_WEIGHT_BIT)
#define VERTEX_USERDATA_SIZE( _n ) ((_n) << USER_DATA_SIZE_BIT)
#define VERTEX_TEXCOORD_MASK( _coord ) (( 0x7ULL ) << ( TEX_COORD_SIZE_BIT + 3 * (_coord) ))

#define VERTEX_TEXCOORD0_2D ( ( (uint64_t) 2 ) << ( TEX_COORD_SIZE_BIT + ( 3*0 ) ) )

enum MaterialVertexFormat_t
{
	MATERIAL_VERTEX_FORMAT_MODEL_SKINNED = ( VertexFormat_t )VERTEX_POSITION | VERTEX_COLOR | VERTEX_NORMAL | VERTEX_TEXCOORD0_2D | VERTEX_BONEWEIGHT( 2 ) | VERTEX_BONE_INDEX | VERTEX_USERDATA_SIZE( 4 ),
	MATERIAL_VERTEX_FORMAT_MODEL_SKINNED_DX7 = ( VertexFormat_t )VERTEX_POSITION | VERTEX_COLOR | VERTEX_NORMAL | VERTEX_TEXCOORD0_2D | VERTEX_BONEWEIGHT( 2 ) | VERTEX_BONE_INDEX,
	MATERIAL_VERTEX_FORMAT_MODEL = ( VertexFormat_t )VERTEX_POSITION | VERTEX_COLOR | VERTEX_NORMAL | VERTEX_TEXCOORD0_2D | VERTEX_USERDATA_SIZE( 4 ),
	MATERIAL_VERTEX_FORMAT_MODEL_DX7 = ( VertexFormat_t )VERTEX_POSITION | VERTEX_COLOR | VERTEX_NORMAL | VERTEX_TEXCOORD0_2D,
	MATERIAL_VERTEX_FORMAT_COLOR = ( VertexFormat_t )VERTEX_SPECULAR
};

enum
{
	USESHADOWLOD = -2
};

#define MAX_DRAW_MODEL_INFO_MATERIALS 8

struct DrawModelResults_t
{
	int m_ActualTriCount;
	int m_TextureMemoryBytes;
	int m_NumHardwareBones;
	int m_NumBatches;
	int m_NumMaterials;
	int m_nLODUsed;
	int m_flLODMetric;
	//CFastTimer m_RenderTime;
	//CUtlVector<IMaterial *> m_Materials;
};

struct ColorTexelsInfo_t
{
	int m_nWidth;
	int m_nHeight;
	int m_nMipmapCount;
	ImageFormat m_ImageFormat;
	int m_nByteCount;
	byte* m_pTexelData;
};

struct GetTriangles_Vertex_t
{
	Vector m_Position;
	Vector m_Normal;
	Vector4D m_TangentS;
	Vector2D m_TexCoord;
	Vector4D m_BoneWeight;
	int m_BoneIndex[ 4 ];
	int m_NumBones;
};

struct GetTriangles_MaterialBatch_t
{
	IMaterial* m_pMaterial;
	CUtlVector<GetTriangles_Vertex_t> m_Verts;
	CUtlVector<int> m_TriListIndices;
};

struct GetTriangles_Output_t
{
	CUtlVector<GetTriangles_MaterialBatch_t> m_MaterialBatches;
	Matrix3x4 m_PoseToWorld[ 128 ];
};

struct model_array_instance_t
{
	Matrix3x4 modelToWorld;
};

class IStudioDataCache : public IAppSystem
{
public:
	virtual bool VerifyHeaders( studiohdr_t* pStudioHdr ) = 0;
	virtual vertexFileHeader_t* CacheVertexData( studiohdr_t* pStudioHdr ) = 0;
};

class IStudioRender : public IAppSystem
{
public:
	virtual void BeginFrame( void ) = 0;
	virtual void EndFrame( void ) = 0;
	virtual void Mat_Stub( IMaterialSystem* pMatSys ) = 0;
	virtual void UpdateConfig( const StudioRenderConfig_t& config ) = 0;
	virtual void GetCurrentConfig( StudioRenderConfig_t& config ) = 0;
	virtual bool LoadModel( studiohdr_t* pStudioHdr, void* pVtxData, studiohwdata_t* pHardwareData ) = 0;
	virtual void UnloadModel( studiohwdata_t* pHardwareData ) = 0;
	virtual void RefreshStudioHdr( studiohdr_t* pStudioHdr, studiohwdata_t* pHardwareData ) = 0;
	virtual void SetEyeViewTarget( const studiohdr_t* pStudioHdr, int nBodyIndex, const Vector& worldPosition ) = 0;
	virtual int GetNumAmbientLightSamples( ) = 0;
	virtual const Vector* GetAmbientLightDirections( ) = 0;
	virtual void SetAmbientLightColors( const Vector4D* pAmbientOnlyColors ) = 0;
	virtual void SetAmbientLightColors( const Vector* pAmbientOnlyColors ) = 0;
	virtual void SetLocalLights( int numLights, const LightDesc_t* pLights ) = 0;
	virtual void SetViewState( const Vector& viewOrigin, const Vector& viewRight, const Vector& viewUp, const Vector& viewPlaneNormal ) = 0;
	virtual void LockFlexWeights( int nWeightCount, float** ppFlexWeights, float** ppFlexDelayedWeights = NULL ) = 0;
	virtual void UnlockFlexWeights( ) = 0;
	virtual Matrix3x4* LockBoneMatrices( int nBoneCount ) = 0;
	virtual void UnlockBoneMatrices( ) = 0;
	virtual int GetNumLODs( const studiohwdata_t& hardwareData ) const = 0;
	virtual float GetLODSwitchValue( const studiohwdata_t& hardwareData, int lod ) const = 0;
	virtual void SetLODSwitchValue( studiohwdata_t& hardwareData, int lod, float switchValue ) = 0;
	virtual void SetColorModulation( float const* pColor ) = 0;
	virtual void SetAlphaModulation( float flAlpha ) = 0;
	virtual void DrawModel( DrawModelResults_t* pResults, const DrawModelInfo_t& info, Matrix3x4* pBoneToWorld, float* pFlexWeights, float* pFlexDelayedWeights, const Vector& modelOrigin, int flags = STUDIORENDER_DRAW_ENTIRE_MODEL ) = 0;
	virtual void DrawModelStaticProp( const DrawModelInfo_t& drawInfo, const Matrix3x4& modelToWorld, int flags = STUDIORENDER_DRAW_ENTIRE_MODEL ) = 0;
	virtual void DrawStaticPropDecals( const DrawModelInfo_t& drawInfo, const Matrix3x4& modelToWorld ) = 0;
	virtual void DrawStaticPropShadows( const DrawModelInfo_t& drawInfo, const Matrix3x4& modelToWorld, int flags ) = 0;
	virtual void ForcedMaterialOverride( IMaterial* newMaterial, OverrideType_t nOverrideType = OVERRIDE_NORMAL ) = 0;
	virtual StudioDecalHandle_t CreateDecalList( studiohwdata_t* pHardwareData ) = 0;
	virtual void DestroyDecalList( StudioDecalHandle_t handle ) = 0;
	virtual void AddDecal( StudioDecalHandle_t handle, studiohdr_t* pStudioHdr, Matrix3x4* pBoneToWorld, const Ray_t& ray, const Vector& decalUp, IMaterial* pDecalMaterial, float radius, int body, bool noPokethru = false, int maxLODToDecal = ADDDECAL_TO_ALL_LODS ) = 0;
	virtual void ComputeLighting( const Vector* pAmbient, int lightCount, LightDesc_t* pLights, const Vector& pt, const Vector& normal, Vector& lighting ) = 0;
	virtual void ComputeLightingConstDirectional( const Vector* pAmbient, int lightCount, LightDesc_t* pLights, const Vector& pt, const Vector& normal, Vector& lighting, float flDirectionalAmount ) = 0;
	virtual void AddShadow( IMaterial* pMaterial, void* pProxyData, FlashlightState_t* m_pFlashlightState = NULL, void* pWorldToTexture = NULL, void* pFlashlightDepthTexture = NULL ) = 0;
	virtual void ClearAllShadows( ) = 0;
	virtual int ComputeModelLod( studiohwdata_t* pHardwareData, float unitSphereSize, float* pMetric = NULL ) = 0;
	virtual void GetPerfStats( DrawModelResults_t* pResults, const DrawModelInfo_t& info, void* pSpewBuf = NULL ) const = 0;
	virtual void GetTriangles( const DrawModelInfo_t& info, Matrix3x4* pBoneToWorld, GetTriangles_Output_t& out ) = 0;
	virtual int GetMaterialList( studiohdr_t* pStudioHdr, int count, IMaterial** ppMaterials ) = 0;
	virtual int GetMaterialListFromBodyAndSkin( MDLHandle_t studio, int nSkin, int nBody, int nCountOutputMaterials, IMaterial** ppOutputMaterials ) = 0;
	virtual void DrawModelArray( const DrawModelInfo_t& drawInfo, int arrayCount, model_array_instance_t* pInstanceData, int instanceStride, int flags = STUDIORENDER_DRAW_ENTIRE_MODEL ) = 0;
	virtual void GetMaterialOverride( IMaterial** ppOutForcedMaterial, OverrideType_t* pOutOverrideType ) = 0;

	inline void SetColorModulation( float r, float g, float b )
	{
		float clr[ 3 ] = { r, g, b };
		SetColorModulation( clr );
	}
	inline void SetColorModulation( Color clr )
	{
		Vector col = { ( float )clr.r / 255, ( float )clr.g / 255, ( float )clr.b / 255 };
		SetColorModulation( &col.x );
	}
};