#pragma once

#include "CoreMinimal.h"
#include "SpatialRootDiagnostics.generated.h"

UENUM(BlueprintType)
enum class ESpatialRootTransportState : uint8
{
    Stopped,
    Running,
    Paused
};

UENUM(BlueprintType)
enum class ESpatialRootAudioPathMode : uint8
{
    Unknown,
    UEProceduralSource,
    SpatialRootOwnsDevice,
    StubTestGeneratorOnly,
    FailedNotConnected
};

USTRUCT(BlueprintType)
struct SPATIALROOTHOST_API FSpatialRootDiagnostics
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "Spatial Root")
    bool bEngineInitialized = false;

    UPROPERTY(BlueprintReadOnly, Category = "Spatial Root")
    FString AdmPath;

    UPROPERTY(BlueprintReadOnly, Category = "Spatial Root")
    FString LayoutPath;

    UPROPERTY(BlueprintReadOnly, Category = "Spatial Root")
    bool bAdmLoaded = false;

    UPROPERTY(BlueprintReadOnly, Category = "Spatial Root")
    bool bLayoutLoaded = false;

    UPROPERTY(BlueprintReadOnly, Category = "Spatial Root")
    ESpatialRootTransportState TransportState = ESpatialRootTransportState::Stopped;

    UPROPERTY(BlueprintReadOnly, Category = "Spatial Root")
    int32 UnrealSampleRate = 0;

    UPROPERTY(BlueprintReadOnly, Category = "Spatial Root")
    int32 UnrealOutputChannelCount = 0;

    UPROPERTY(BlueprintReadOnly, Category = "Spatial Root")
    int32 SpatialRootRequiredOutputChannelCount = 0;

    UPROPERTY(BlueprintReadOnly, Category = "Spatial Root")
    int32 RenderedChannelCount = 0;

    UPROPERTY(BlueprintReadOnly, Category = "Spatial Root")
    int32 UnderrunCount = 0;

    UPROPERTY(BlueprintReadOnly, Category = "Spatial Root")
    FString LastError;

    UPROPERTY(BlueprintReadOnly, Category = "Spatial Root")
    FString LastOperation;

    UPROPERTY(BlueprintReadOnly, Category = "Spatial Root")
    ESpatialRootAudioPathMode AudioPathMode = ESpatialRootAudioPathMode::Unknown;
};

