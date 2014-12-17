#pragma once

#include "PrimitiveSceneProxy.h"


// VertexBuffer
class FSsVertexBuffer : public FVertexBuffer
{
public:
	virtual void InitRHI() override;
	int32 NumVerts;
};

// IndexBuffer
class FSsIndexBuffer : public FIndexBuffer
{
public:
	virtual void InitRHI() override;
	int32 NumIndices;
};

// VertexFactory
class FSsVertexFactory : public FLocalVertexFactory
{
public:
	void Init(const FSsVertexBuffer* VertexBuffer);
};


// RenderProxy
class FSsRenderSceneProxy : public FPrimitiveSceneProxy
{
public:
	FSsRenderSceneProxy(class USsPlayerComponent* InComponent, UMaterialInterface* InMaterial);
	virtual ~FSsRenderSceneProxy();

	// FPrimitiveSceneProxy interface
	virtual void DrawDynamicElements(FPrimitiveDrawInterface* PDI, const FSceneView* View) override;
	virtual FPrimitiveViewRelevance GetViewRelevance(const FSceneView* View) override;
	virtual void OnTransformChanged() override;
	virtual uint32 GetMemoryFootprint() const override;


	void SetDynamicData_RenderThread();
	void SetMaterial(UMaterialInterface* InMaterial) { Material = InMaterial; }

public:
	FVector2D MeshPlaneSize;

private:
	FVector Origin;

	USsPlayerComponent* Component;

	UPROPERTY()
	UMaterialInterface* Material;

	FSsVertexBuffer     VertexBuffer;
	FSsIndexBuffer      IndexBuffer;
	FSsVertexFactory    VertexFactory;
};
