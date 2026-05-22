#pragma once

#include "CoreMinimal.h"
#include "Components/SynthComponent.h"
#include "SpatialRootTestToneComponent.generated.h"

UCLASS(ClassGroup = "Audio", Blueprintable, meta = (BlueprintSpawnableComponent))
class SPATIALROOTHOST_API USpatialRootTestToneComponent : public USynthComponent
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spatial Root|Audio", meta = (ClampMin = "20.0", ClampMax = "20000.0"))
    float FrequencyHz = 440.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spatial Root|Audio", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float LinearGain = 0.05f;

protected:
    bool Init(int32& SampleRate) override;
    int32 OnGenerateAudio(float* OutAudio, int32 NumSamples) override;

private:
    double Phase = 0.0;
    double PhaseIncrement = 0.0;
    int32 CachedSampleRate = 48000;
};

