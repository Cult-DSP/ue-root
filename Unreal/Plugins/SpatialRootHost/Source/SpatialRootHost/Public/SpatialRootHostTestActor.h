#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpatialRootDiagnostics.h"
#include "SpatialRootHostTestActor.generated.h"

class USceneComponent;
class USpatialRootBridge;
class USpatialRootRenderBusComponent;
class USpatialRootTestToneComponent;

UCLASS(Blueprintable)
class SPATIALROOTHOST_API ASpatialRootHostTestActor : public AActor
{
    GENERATED_BODY()

public:
    ASpatialRootHostTestActor();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spatial Root")
    TObjectPtr<USceneComponent> SceneRoot;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spatial Root|Audio")
    TObjectPtr<USpatialRootTestToneComponent> TestTone;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spatial Root|Audio")
    TObjectPtr<USpatialRootRenderBusComponent> RenderBus;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spatial Root|Input")
    FString AdmBwfPath;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spatial Root|Input")
    FString LusidScenePath = TEXT("/Users/lucian/projects/spatialroot/sourceData/lusid_package/scene.lusid.json");

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spatial Root|Input")
    FString LayoutPath;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spatial Root|Audio", meta = (ClampMin = "1", ClampMax = "64"))
    int32 RequestedChannelCount = 18;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spatial Root|Audio", meta = (ClampMin = "1", ClampMax = "8192"))
    int32 HostBusBlockSize = 512;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spatial Root|Audio", meta = (ClampMin = "8000", ClampMax = "384000"))
    int32 HostBusSampleRate = 48000;

    UFUNCTION(BlueprintCallable, Category = "Spatial Root")
    bool InitializeBridge();

    UFUNCTION(BlueprintCallable, Category = "Spatial Root|Audio")
    bool StartTestTone();

    UFUNCTION(BlueprintCallable, Category = "Spatial Root|Audio")
    void StopTestTone();

    UFUNCTION(BlueprintCallable, Category = "Spatial Root|Audio")
    bool StartSpatialRootHostBus();

    UFUNCTION(BlueprintCallable, Category = "Spatial Root|Audio")
    void StopSpatialRootHostBus();

    UFUNCTION(BlueprintCallable, Category = "Spatial Root")
    void PauseSpatialRoot();

    UFUNCTION(BlueprintCallable, Category = "Spatial Root")
    void ResumeSpatialRoot();

    UFUNCTION(BlueprintPure, Category = "Spatial Root")
    USpatialRootBridge* GetBridge() const;

    UFUNCTION(BlueprintPure, Category = "Spatial Root")
    FSpatialRootDiagnostics GetDiagnostics() const;

protected:
    void BeginPlay() override;

private:
    bool ApplyInputPaths();

    UPROPERTY()
    TObjectPtr<USpatialRootBridge> Bridge;
};
