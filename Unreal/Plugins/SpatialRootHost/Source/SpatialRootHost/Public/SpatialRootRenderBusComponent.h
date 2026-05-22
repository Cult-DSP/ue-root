#pragma once

#include "CoreMinimal.h"
#include "Components/SynthComponent.h"
#include "SpatialRootRenderBusComponent.generated.h"

class USpatialRootBridge;

UCLASS(ClassGroup = "Audio", Blueprintable, meta = (BlueprintSpawnableComponent))
class SPATIALROOTHOST_API USpatialRootRenderBusComponent : public USynthComponent
{
    GENERATED_BODY()

public:
    USpatialRootRenderBusComponent(const FObjectInitializer& ObjectInitializer);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spatial Root|Audio", meta = (ClampMin = "1", ClampMax = "64"))
    int32 RequestedChannelCount = 18;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spatial Root|Audio")
    bool bUseSpatialRootHostBus = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spatial Root|Audio")
    TObjectPtr<USpatialRootBridge> Bridge;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spatial Root|Audio")
    bool bOutputSilenceWhenEmpty = true;

    UFUNCTION(BlueprintCallable, Category = "Spatial Root|Audio")
    void ConfigureRenderBus(int32 InChannelCount);

    UFUNCTION(BlueprintCallable, Category = "Spatial Root|Audio")
    void ClearQueuedAudio();

    void PushInterleavedAudio(const float* InterleavedAudio, int32 NumFrames, int32 NumChannels);

    UFUNCTION(BlueprintPure, Category = "Spatial Root|Audio")
    int32 GetConfiguredChannelCount() const;

    UFUNCTION(BlueprintPure, Category = "Spatial Root|Audio")
    int32 GetUnderrunCount() const;

protected:
    bool Init(int32& SampleRate) override;
    int32 OnGenerateAudio(float* OutAudio, int32 NumSamples) override;

private:
    FCriticalSection QueueCriticalSection;
    TArray<float> InterleavedQueue;
    int32 ConfiguredChannelCount = 18;
    int32 CachedSampleRate = 48000;
    int32 UnderrunCount = 0;
};
