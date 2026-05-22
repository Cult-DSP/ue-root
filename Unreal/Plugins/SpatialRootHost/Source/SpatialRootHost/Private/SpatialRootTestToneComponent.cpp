#include "SpatialRootTestToneComponent.h"

#include "SpatialRootHostModule.h"

bool USpatialRootTestToneComponent::Init(int32& SampleRate)
{
    CachedSampleRate = SampleRate > 0 ? SampleRate : 48000;
    Phase = 0.0;
    PhaseIncrement = 2.0 * UE_DOUBLE_PI * static_cast<double>(FrequencyHz) / static_cast<double>(CachedSampleRate);
    UE_LOG(LogSpatialRootAudio, Log, TEXT("Spatial Root test tone initialized at %d Hz."), CachedSampleRate);
    return true;
}

int32 USpatialRootTestToneComponent::OnGenerateAudio(float* OutAudio, int32 NumSamples)
{
    PhaseIncrement = 2.0 * UE_DOUBLE_PI * static_cast<double>(FrequencyHz) / static_cast<double>(CachedSampleRate);

    for (int32 Index = 0; Index < NumSamples; ++Index)
    {
        OutAudio[Index] = static_cast<float>(FMath::Sin(Phase)) * LinearGain;
        Phase += PhaseIncrement;
        if (Phase >= 2.0 * UE_DOUBLE_PI)
        {
            Phase -= 2.0 * UE_DOUBLE_PI;
        }
    }

    return NumSamples;
}

