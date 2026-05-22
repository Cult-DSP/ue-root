#include "SpatialRootRenderBusComponent.h"

#include "SpatialRootBridge.h"
#include "SpatialRootHostModule.h"

USpatialRootRenderBusComponent::USpatialRootRenderBusComponent(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    NumChannels = ConfiguredChannelCount;
    bEnableBaseSubmix = true;
    bEnableSubmixSends = true;
    bAllowSpatialization = false;
}

void USpatialRootRenderBusComponent::ConfigureRenderBus(int32 InChannelCount)
{
    RequestedChannelCount = FMath::Clamp(InChannelCount, 1, 64);
    ConfiguredChannelCount = RequestedChannelCount;
    NumChannels = ConfiguredChannelCount;
    if (Bridge)
    {
        Bridge->SetUnrealAudioFormat(CachedSampleRate, ConfiguredChannelCount);
    }
}

void USpatialRootRenderBusComponent::ClearQueuedAudio()
{
    FScopeLock Lock(&QueueCriticalSection);
    InterleavedQueue.Reset();
}

void USpatialRootRenderBusComponent::PushInterleavedAudio(const float* InterleavedAudio, int32 NumFrames, int32 NumChannelsIn)
{
    if (!InterleavedAudio || NumFrames <= 0 || NumChannelsIn <= 0)
    {
        return;
    }

    FScopeLock Lock(&QueueCriticalSection);
    const int32 FramesToAppend = NumFrames;
    const int32 OldNum = InterleavedQueue.Num();
    InterleavedQueue.AddZeroed(FramesToAppend * ConfiguredChannelCount);

    for (int32 Frame = 0; Frame < FramesToAppend; ++Frame)
    {
        for (int32 Channel = 0; Channel < ConfiguredChannelCount; ++Channel)
        {
            const int32 SrcChannel = Channel < NumChannelsIn ? Channel : INDEX_NONE;
            InterleavedQueue[OldNum + Frame * ConfiguredChannelCount + Channel] =
                SrcChannel == INDEX_NONE ? 0.0f : InterleavedAudio[Frame * NumChannelsIn + SrcChannel];
        }
    }
}

int32 USpatialRootRenderBusComponent::GetConfiguredChannelCount() const
{
    return ConfiguredChannelCount;
}

int32 USpatialRootRenderBusComponent::GetUnderrunCount() const
{
    return UnderrunCount;
}

bool USpatialRootRenderBusComponent::Init(int32& SampleRate)
{
    CachedSampleRate = SampleRate > 0 ? SampleRate : 48000;
    ConfigureRenderBus(RequestedChannelCount);
    if (Bridge)
    {
        Bridge->SetUnrealAudioFormat(CachedSampleRate, ConfiguredChannelCount);
    }
    UE_LOG(LogSpatialRootAudio, Log, TEXT("Spatial Root UE render bus initialized: %d channels at %d Hz."), ConfiguredChannelCount, CachedSampleRate);
    return true;
}

int32 USpatialRootRenderBusComponent::OnGenerateAudio(float* OutAudio, int32 NumSamples)
{
    if (!OutAudio || NumSamples <= 0)
    {
        return 0;
    }

    FMemory::Memzero(OutAudio, NumSamples * sizeof(float));

    if (bUseSpatialRootHostBus && Bridge)
    {
        const int32 Frames = ConfiguredChannelCount > 0 ? (NumSamples / ConfiguredChannelCount) : 0;
        if (Frames > 0 && NumSamples == Frames * ConfiguredChannelCount)
        {
            if (Bridge->RenderHostAudio(OutAudio, Frames, ConfiguredChannelCount, CachedSampleRate))
            {
                return NumSamples;
            }
        }
    }

    FScopeLock Lock(&QueueCriticalSection);
    const int32 SamplesToCopy = FMath::Min(NumSamples, InterleavedQueue.Num());
    if (SamplesToCopy > 0)
    {
        FMemory::Memcpy(OutAudio, InterleavedQueue.GetData(), SamplesToCopy * sizeof(float));
        InterleavedQueue.RemoveAt(0, SamplesToCopy, EAllowShrinking::No);
    }

    if (SamplesToCopy < NumSamples)
    {
        ++UnderrunCount;
        if (!bOutputSilenceWhenEmpty)
        {
            return SamplesToCopy;
        }
    }

    return NumSamples;
}
