#pragma once

#include "Engine/CanvasRenderTarget2D.h"
#include "SsPlayerTickResult.h"

class USsProject;
class FSsPlayer;
class FSsRenderSceneProxy;

#include "SsPlayerComponent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSsEndPlaySignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FSsUserDataSignature, FName, PartName, int32, PartIndex, int32, KeyFrame, FSsUserDataValue, UserData);


USTRUCT()
struct FAttachComponent
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(Category=AttachComponent, BlueprintReadOnly)
	int32 PartIndex;

	UPROPERTY(Category=AttachComponent, BlueprintReadOnly)
	TWeakObjectPtr<USceneComponent> Component;
};


UCLASS(meta=(BlueprintSpawnableComponent))
class USsPlayerComponent : public UMeshComponent
{
	GENERATED_UCLASS_BODY()

public:
	// UActorComponent interface
	virtual void OnRegister() override;
	virtual void InitializeComponent() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void SendRenderDynamicData_Concurrent() override;

	// UPrimitiveComponent interface
	virtual FPrimitiveSceneProxy* CreateSceneProxy() override;
	virtual FBoxSphereBounds CalcBounds(const FTransform& LocalToWorld) const override;

	// AutoUpdate=false の場合の更新 
	UFUNCTION(Category=SpriteStudio, BlueprintCallable)
	void UpdatePlayer(float DeltaTime);

	// 指定したCanvasに対して描画する. 自前で用意したCanvasへ描画する際に使用する. 
	void DrawToCanvas(class FCanvas* Canvas, FVector2D Location, float Rotation=0.f, FVector2D Scale=FVector2D(1.f, 1.f), bool bMask=false);

	// 描画対象テクスチャを取得
	UFUNCTION(Category=SpriteStudio, BlueprintPure)
	UTexture* GetRenderTarget();

	// パーツのTransformを取得(Actorからの相対座標系. bDrawMesh=trueの場合のみ有効)
	UFUNCTION(Category=SpriteStudio, BlueprintCallable)
	FTransform GetPartTransformInAutoDrawMesh(int32 PartIndex) const;

	// アタッチコンポーネントの追加
	UFUNCTION(Category=SpriteStudio, BlueprintCallable)
	void AddAttachComponent(int32 PartIndex, USceneComponent* Component);

	// アタッチコンポーネントの追加（名前指定）
	UFUNCTION(Category=SpriteStudio, BlueprintCallable)
	void AddAttachComponentByName(FName PartName, USceneComponent* Component);


protected:
	UFUNCTION()
	void OnDraw(class UCanvas* Canvas, int32 Width, int32 Height);

	UFUNCTION()
	void OnDrawMask(class UCanvas* Canvas, int32 Width, int32 Height);

public:
	// 再生するSsProject
	UPROPERTY(Category=SpriteStudioAsset, EditDefaultsOnly, BlueprintReadOnly, meta=(DisplayThumbnail="true"))
	USsProject* SsProject;


	// 自動更新. Offにする場合は自前でUpdatePlayer()を呼び出して更新する. 
	UPROPERTY(Category=SpriteStudioPlaySettings, EditDefaultsOnly, BlueprintReadOnly)
	bool bAutoUpdate;

	// 自動再生. 
	UPROPERTY(Category=SpriteStudioPlaySettings, EditDefaultsOnly, BlueprintReadOnly)
	bool bAutoPlay;

	// 自動再生時のAnimPackIndex
	UPROPERTY(Category=SpriteStudioPlaySettings, EditDefaultsOnly, BlueprintReadOnly)
	int32 AutoPlayAnimPackIndex;

	// 自動再生時のAnimationIndex
	UPROPERTY(Category=SpriteStudioPlaySettings, EditDefaultsOnly, BlueprintReadOnly)
	int32 AutoPlayAnimationIndex;

	// 自動再生時の開始フレーム
	UPROPERTY(Category=SpriteStudioPlaySettings, EditDefaultsOnly, BlueprintReadOnly, AdvancedDisplay)
	int32 AutoPlayStartFrame;

	// 自動再生時の再生速度
	UPROPERTY(Category=SpriteStudioPlaySettings, EditDefaultsOnly, BlueprintReadOnly, AdvancedDisplay)
	float AutoPlayRate;

	// 自動再生時のループ回数
	UPROPERTY(Category=SpriteStudioPlaySettings, EditDefaultsOnly, BlueprintReadOnly, AdvancedDisplay)
	int32 AutoPlayLoopCount;

	// 自動再生時の往復再生
	UPROPERTY(Category=SpriteStudioPlaySettings, EditDefaultsOnly, BlueprintReadOnly, AdvancedDisplay)
	bool bAutoPlayRoundTrip;


	// メッシュの描画. Offにする場合は自前でCanvasを用意して、DrawToCanvas()を呼び出して描画する. 
	UPROPERTY(Category=SpriteStudioDrawSettings, EditDefaultsOnly, BlueprintReadOnly)
	bool bDrawMesh;

	// ベースマテリアル
	UPROPERTY(Category=SpriteStudioDrawSettings, EditDefaultsOnly, BlueprintReadOnly, meta=(DisplayThumbnail="true"))
	UMaterialInterface* BaseMaterial;

	// メッシュの描画に使用するMID
	UPROPERTY(Category=SpriteStudioDrawSettings, BlueprintReadOnly)
	UMaterialInstanceDynamic* Material;

	// メッシュの描画の際のCanvasの解像度. 
	UPROPERTY(Category=SpriteStudioDrawSettings, EditDefaultsOnly, BlueprintReadOnly)
	FVector2D CanvasResolution;

	// メッシュの描画の際の描画スケール. 
	UPROPERTY(Category=SpriteStudioDrawSettings, EditDefaultsOnly, BlueprintReadOnly)
	FVector2D MeshDrawScale;

	// メッシュの描画の際のPlaneメッシュのサイズ
	UPROPERTY(Category=SpriteStudioDrawSettings, EditDefaultsOnly, BlueprintReadOnly)
	FVector2D MeshPlaneSize;


	// アタッチコンポーネントリスト
	UPROPERTY(Category=SpriteStudioAttachComponent, BlueprintReadWrite)
	TArray<FAttachComponent> AttachComponents;


	// 再生終了イベント // C++から利用する際は、ココに直接Delegateを追加して下さい.
	UPROPERTY(BlueprintAssignable, Category=SpriteStudioCallback)
	FSsEndPlaySignature OnSsEndPlay;

	// ユーザーデータイベント // C++から利用する際は、ココに直接Delegateを追加して下さい.
	UPROPERTY(BlueprintAssignable, Category=SpriteStudioCallback)
	FSsUserDataSignature OnSsUserData;

protected:
	TSharedPtr<FSsPlayer> Player;
	UCanvasRenderTarget2D* RenderTarget;
	UCanvasRenderTarget2D* RenderTargetMask;

	//
	// MEMO:
	//	CanvasRenderTarget2Dのバグ(？)で、アルファ値がFCanvasへの描画と同じ結果にならず、正常に反映されない。 
	//	SE_BLEND_MaskedDistanceFieldでマスクを作り、アルファ値をここから取得することで、メッシュへ描画する際の結果を合わせる。 
	//	v1.0の正式リリースまでにはどうにかしたいが、FCanvasRenderTarget2Dの使用自体を見送って別の方法を探すかも...
	//

private:
	void InitializePlayerRender();
#if WITH_EDITOR
	bool bPlayerRenderInitialized;
#endif

public:
	// Blueprint interface 暫定

	UFUNCTION(Category=SpriteStudio, BlueprintCallable, meta=(AdvancedDisplay="2"))
	bool Play(int32 AnimPackIndex, int32 AnimationIndex, int32 StartFrame=0, float PlayRate=1.f, int32 LoopCount=0, bool bRoundTrip=false);

	UFUNCTION(Category=SpriteStudio, BlueprintCallable, meta=(AdvancedDisplay="2"))
	bool PlayByName(const FName& AnimPackName, const FName& AnimationName, int32 StartFrame=0, float PlayRate=1.f, int32 LoopCount=0, bool bRoundTrip=false);

	UFUNCTION(Category=SpriteStudio, BlueprintCallable)
	void Pause();

	UFUNCTION(Category=SpriteStudio, BlueprintCallable)
	bool Resume();

	UFUNCTION(Category=SpriteStudio, BlueprintCallable)
	bool IsPlaying() const;

	UFUNCTION(Category=SpriteStudio, BlueprintCallable)
	int32 GetNumAnimPacks() const;

	UFUNCTION(Category=SpriteStudio, BlueprintCallable)
	int32 GetNumAnimations(int32 AnimPackIndex) const;

	UFUNCTION(Category=SpriteStudio, BlueprintCallable)
	void SetPlayFrame(float Frame);

	UFUNCTION(Category=SpriteStudio, BlueprintCallable)
	float GetPlayFrame() const;

	UFUNCTION(Category=SpriteStudio, BlueprintCallable)
	void SetLoopCount(int32 InLoopCount=0);

	UFUNCTION(Category=SpriteStudio, BlueprintCallable)
	int32 GetLoopCount() const;

	UFUNCTION(Category=SpriteStudio, BlueprintCallable)
	void SetRoundTrip(bool bInRoundTrip);

	UFUNCTION(Category=SpriteStudio, BlueprintCallable)
	bool IsRoundTrip() const;

	UFUNCTION(Category=SpriteStudio, BlueprintCallable)
	void SetPlayRate(float InRate);

	UFUNCTION(Category=SpriteStudio, BlueprintCallable)
	float GetPlayRate() const;

	UFUNCTION(Category=SpriteStudio, BlueprintCallable)
	void SetFlipH(bool InFlipH);

	UFUNCTION(Category=SpriteStudio, BlueprintCallable)
	bool GetFlipH() const;

	UFUNCTION(Category=SpriteStudio, BlueprintCallable)
	void SetFlipV(bool InFlipV);

	UFUNCTION(Category=SpriteStudio, BlueprintCallable)
	bool GetFlipV() const;

	UFUNCTION(Category=SpriteStudio, BlueprintCallable)
	void AddTextureReplacement(int32 PartIndex, UTexture* Texture);

	UFUNCTION(Category=SpriteStudio, BlueprintCallable)
	void AddTextureReplacementByName(FName PartName, UTexture* Texture);

	UFUNCTION(Category=SpriteStudio, BlueprintCallable)
	void RemoveTextureReplacement(int32 PartIndex);

	UFUNCTION(Category=SpriteStudio, BlueprintCallable)
	void RemoveTextureReplacementByName(FName PartName);

	UFUNCTION(Category=SpriteStudio, BlueprintCallable)
	void RemoveTextureReplacementAll();

};
