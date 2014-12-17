#include "SpriteStudio5PrivatePCH.h"
#include "SsRenderSceneProxy.h"

#include "DynamicMeshBuilder.h"
#include "SsPlayerComponent.h"



// VertexBuffer
void FSsVertexBuffer::InitRHI()
{
	FRHIResourceCreateInfo CreateInfo;
	VertexBufferRHI = RHICreateVertexBuffer(NumVerts * sizeof(FDynamicMeshVertex), BUF_Dynamic, CreateInfo);
}

// IndexBuffer
void FSsIndexBuffer::InitRHI()
{
	FRHIResourceCreateInfo CreateInfo;
	IndexBufferRHI = RHICreateIndexBuffer(sizeof(int32), NumIndices * sizeof(int32), BUF_Dynamic, CreateInfo);
}

// VertexFactory
void FSsVertexFactory::Init(const FSsVertexBuffer* VertexBuffer)
{
	if(IsInRenderingThread())
	{
		FLocalVertexFactory::DataType Data;
		Data.PositionComponent = STRUCTMEMBER_VERTEXSTREAMCOMPONENT(VertexBuffer, FDynamicMeshVertex, Position, VET_Float3);
		Data.TextureCoordinates.Add(
			FVertexStreamComponent(VertexBuffer, STRUCT_OFFSET(FDynamicMeshVertex,TextureCoordinate), sizeof(FDynamicMeshVertex), VET_Float2)
			);
		Data.TangentBasisComponents[0] = STRUCTMEMBER_VERTEXSTREAMCOMPONENT(VertexBuffer, FDynamicMeshVertex, TangentX, VET_PackedNormal);
		Data.TangentBasisComponents[1] = STRUCTMEMBER_VERTEXSTREAMCOMPONENT(VertexBuffer, FDynamicMeshVertex, TangentZ, VET_PackedNormal);
		Data.ColorComponent = STRUCTMEMBER_VERTEXSTREAMCOMPONENT(VertexBuffer, FDynamicMeshVertex, Color, VET_Color);
		SetData(Data);
	}
	else
	{
		ENQUEUE_UNIQUE_RENDER_COMMAND_TWOPARAMETER(
			InitSsVertexFactory,
			FSsVertexFactory*, VertexFactory, this,
			const FSsVertexBuffer*, VertexBuffer, VertexBuffer,
		{
			// Initialize the vertex factory's stream components.
			DataType Data;
			Data.PositionComponent = STRUCTMEMBER_VERTEXSTREAMCOMPONENT(VertexBuffer,FDynamicMeshVertex,Position,VET_Float3);
			Data.TextureCoordinates.Add(
				FVertexStreamComponent(VertexBuffer, STRUCT_OFFSET(FDynamicMeshVertex,TextureCoordinate), sizeof(FDynamicMeshVertex), VET_Float2)
				);
			Data.TangentBasisComponents[0] = STRUCTMEMBER_VERTEXSTREAMCOMPONENT(VertexBuffer, FDynamicMeshVertex, TangentX, VET_PackedNormal);
			Data.TangentBasisComponents[1] = STRUCTMEMBER_VERTEXSTREAMCOMPONENT(VertexBuffer, FDynamicMeshVertex, TangentZ, VET_PackedNormal);
			Data.ColorComponent = STRUCTMEMBER_VERTEXSTREAMCOMPONENT(VertexBuffer, FDynamicMeshVertex, Color, VET_Color);
			VertexFactory->SetData(Data);
		});
	}
}



// コンストラクタ
FSsRenderSceneProxy::FSsRenderSceneProxy(USsPlayerComponent* InComponent, UMaterialInterface* InMaterial)
	: FPrimitiveSceneProxy(InComponent)
{
	// FPrimitiveSceneProxy
	bWillEverBeLit = false;

	Component = InComponent;
	Material = InMaterial;

	VertexBuffer.NumVerts  = 4;
	IndexBuffer.NumIndices = 6;
	VertexFactory.Init(&VertexBuffer);

	BeginInitResource(&VertexBuffer);
	BeginInitResource(&IndexBuffer);
	BeginInitResource(&VertexFactory);
}

// デストラクタ
FSsRenderSceneProxy::~FSsRenderSceneProxy()
{
	VertexBuffer.ReleaseResource();
	IndexBuffer.ReleaseResource();
	VertexFactory.ReleaseResource();
}

void FSsRenderSceneProxy::DrawDynamicElements(FPrimitiveDrawInterface* PDI, const FSceneView* View)
{
	QUICK_SCOPE_CYCLE_COUNTER(STAT_SsRenderSceneProxy_DrawDynamicElements);

	const bool bWireframe = AllowDebugViewmodes() && View->Family->EngineShowFlags.Wireframe;
	FColoredMaterialRenderProxy WireframeMaterialInstance(
		GEngine->WireframeMaterial ? GEngine->WireframeMaterial->GetRenderProxy(IsSelected()) : NULL,
		FLinearColor(0, 0.5f, 1.f)
		);

	FMaterialRenderProxy* MaterialProxy = NULL;
	if(bWireframe)
	{
		MaterialProxy = &WireframeMaterialInstance;
	}
	else
	{
		if(NULL == Material)
		{
			return;
		}
		MaterialProxy = Material->GetRenderProxy(IsSelected());
	}

	FMeshBatch Mesh;
	FMeshBatchElement& BatchElement = Mesh.Elements[0];
	BatchElement.IndexBuffer = &IndexBuffer;
	Mesh.bDisableBackfaceCulling = true;
	Mesh.bWireframe = bWireframe;
	Mesh.VertexFactory = &VertexFactory;
	Mesh.MaterialRenderProxy = MaterialProxy;
#if defined(SS_UE4_4)
	BatchElement.PrimitiveUniformBuffer = CreatePrimitiveUniformBufferImmediate(GetLocalToWorld(), GetBounds(), GetLocalBounds(), true);
#else
	BatchElement.PrimitiveUniformBuffer = CreatePrimitiveUniformBufferImmediate(GetLocalToWorld(), GetBounds(), GetLocalBounds(), true, UseEditorDepthTest());
#endif
	BatchElement.FirstIndex = 0;
	BatchElement.NumPrimitives = IndexBuffer.NumIndices / 3;
	BatchElement.MinVertexIndex = 0;
	BatchElement.MaxVertexIndex = VertexBuffer.NumVerts;
	Mesh.ReverseCulling = IsLocalToWorldDeterminantNegative();
	Mesh.Type = PT_TriangleList;
	Mesh.DepthPriorityGroup = SDPG_World;
	PDI->DrawMesh(Mesh);
}


FPrimitiveViewRelevance FSsRenderSceneProxy::GetViewRelevance(const FSceneView* View)
{
	// どこかでちゃんと精査しないと・・・
	FPrimitiveViewRelevance Result;
	Result.bDrawRelevance = IsShown(View);
	Result.bOpaqueRelevance = true;
	Result.bNormalTranslucencyRelevance = false;
	Result.bDynamicRelevance = true;
	Result.bShadowRelevance = IsShadowCast(View);
	Result.bSeparateTranslucencyRelevance = true;
//	Result.bEditorPrimitiveRelevance = UseEditorCompositing(View);
	return Result;
}

void FSsRenderSceneProxy::OnTransformChanged()
{
	Origin = GetLocalToWorld().GetOrigin();
}

uint32 FSsRenderSceneProxy::GetMemoryFootprint() const
{
	return sizeof(*this) + GetAllocatedSize();
}

void FSsRenderSceneProxy::SetDynamicData_RenderThread()
{
	const int32 NumVerts = 4;
	FDynamicMeshVertex Vertices[NumVerts];
	Vertices[0] = FDynamicMeshVertex(FVector(0.f, -MeshPlaneSize.X/2.f, MeshPlaneSize.Y), FVector(1.f, 0.f, 0.f), FVector(0.f, 0.f, 1.f), FVector2D(0.f, 0.f), FColor(255, 255, 255, 255));
	Vertices[1] = FDynamicMeshVertex(FVector(0.f,  MeshPlaneSize.X/2.f, MeshPlaneSize.Y), FVector(1.f, 0.f, 0.f), FVector(0.f, 0.f, 1.f), FVector2D(1.f, 0.f), FColor(255, 255, 255, 255));
	Vertices[2] = FDynamicMeshVertex(FVector(0.f, -MeshPlaneSize.X/2.f,             0.f), FVector(1.f, 0.f, 0.f), FVector(0.f, 0.f, 1.f), FVector2D(0.f, 1.f), FColor(255, 255, 255, 255));
	Vertices[3] = FDynamicMeshVertex(FVector(0.f,  MeshPlaneSize.X/2.f,             0.f), FVector(1.f, 0.f, 0.f), FVector(0.f, 0.f, 1.f), FVector2D(1.f, 1.f), FColor(255, 255, 255, 255));
	void* VertexBufferData = RHILockVertexBuffer(VertexBuffer.VertexBufferRHI, 0, NumVerts * sizeof(FDynamicMeshVertex), RLM_WriteOnly);
	FMemory::Memcpy(VertexBufferData, &Vertices[0], NumVerts * sizeof(FDynamicMeshVertex));
	RHIUnlockVertexBuffer(VertexBuffer.VertexBufferRHI);
	

	const int32 NumIndices = 6;
	int Indices[NumIndices] = {0, 2, 1, 1, 2, 3};
	void* IndexBufferData = RHILockIndexBuffer(IndexBuffer.IndexBufferRHI, 0, NumIndices * sizeof(int32), RLM_WriteOnly);
	FMemory::Memcpy(IndexBufferData, &Indices[0], NumIndices * sizeof(int32));
	RHIUnlockIndexBuffer(IndexBuffer.IndexBufferRHI);
}
